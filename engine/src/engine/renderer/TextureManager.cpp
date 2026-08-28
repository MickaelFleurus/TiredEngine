#include "engine/renderer/TextureManager.h"

#include <SDL3/SDL_surface.h>

#include "engine/utils/AssetParser.h"
#include "engine/utils/FileHandler.h"
#include "engine/utils/Logger.h"
#include "engine/vulkan/BufferHandler.h"
#include "engine/vulkan/Constants.h"
#include "engine/vulkan/StagingBuffer.h"
#include "engine/vulkan/VulkanContext.h"
#include "engine/vulkan/VulkanRendering.h"

namespace {
constexpr auto kGPUSurfaceDeleter = [](SDL_Surface* surface) {
    if (surface) {
        SDL_DestroySurface(surface);
    }
};

uint32_t GetBytesPerPixel(SDL_PixelFormat format) {
    return SDL_BYTESPERPIXEL(format);
}

void transitionImageLayout(Vulkan::CStagingBuffer& uploader, VkImage image,
                           VkFormat format, VkImageLayout oldLayout,
                           VkImageLayout newLayout) {
    VkImageMemoryBarrier barrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

    VkPipelineStageFlags srcStage, dstStage;
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else { // TRANSFER_DST_OPTIMAL -> SHADER_READ_ONLY_OPTIMAL
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }

    vkCmdPipelineBarrier(uploader.GetCmdBuffer(), srcStage, dstStage, 0, 0,
                         nullptr, 0, nullptr, 1, &barrier);
}

Renderer::LoadedTexture
createTextureFromPixels(VmaAllocator allocator, VkDevice device,
                        Vulkan::CStagingBuffer& uploader, const void* pixels,
                        uint32_t width, uint32_t height, VkFormat format) {
    VkDeviceSize imageSize =
        VkDeviceSize(width) * height * 4; // assumes 4 bytes/pixel (RGBA8)

    VkImageCreateInfo imageInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = format;
    imageInfo.extent = {width, height, 1};
    imageInfo.mipLevels = 1; // FIXME: Generate mipmaps if needed later
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage =
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

    Renderer::LoadedTexture tex{};
    tex.width = width;
    tex.height = height;
    if (!vmaCreateImage(allocator, &imageInfo, &allocInfo, &tex.image,
                        &tex.allocation, nullptr)) {
        LOG_FATAL("Failed to create Vulkan image!");
    }

    // 1. Transition UNDEFINED -> TRANSFER_DST_OPTIMAL
    transitionImageLayout(uploader, tex.image, format,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // 2. Copy pixel data via staging (extends your existing StagingUploader —
    // see below)
    uploader.UploadToImage(pixels, imageSize, tex.image, width, height);

    // 3. Transition TRANSFER_DST_OPTIMAL -> SHADER_READ_ONLY_OPTIMAL
    transitionImageLayout(uploader, tex.image, format,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    VkImageViewCreateInfo viewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    viewInfo.image = tex.image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    if (vkCreateImageView(device, &viewInfo, nullptr, &tex.imageView)) {
        LOG_FATAL("Failed to create Vulkan image view!");
    }

    return tex;
}

} // namespace

namespace Renderer {
CTextureManager::CTextureManager(const Vulkan::SContext& context,
                                 Vulkan::CSwapchain& swapchain,
                                 Vulkan::CVulkanRendering& renderer,
                                 Renderer::CMemoryAllocator& memoryAllocator,
                                 Vulkan::CBufferHandler& bufferHandler,
                                 Utils::CFileHandler& fileHandler,
                                 const CAssetParser& assetParser)
    : mContext(context)
    , mSwapchain(swapchain)
    , mRenderer(renderer)
    , mMemoryAllocator(memoryAllocator)
    , mBufferHandler(bufferHandler)
    , mFileHandler(fileHandler)
    , mAssetParser(assetParser) {
    mLoadedTextures.reserve(Vulkan::kMaxTextures);
}

CTextureManager::~CTextureManager() {
    for (auto& texture : mLoadedTextures) {
        vkDestroyImageView(mContext.device, texture.imageView, nullptr);
        vmaDestroyImage(mContext.vmaAllocator, texture.image,
                        texture.allocation);
    }
}

int CTextureManager::LoadTexture(const SAsset& asset) {
    std::unique_ptr<SDL_Surface, decltype(kGPUSurfaceDeleter)> surface{
        mFileHandler.LoadTextureFile(asset.mPath.string()), kGPUSurfaceDeleter};
    if (!surface) {
        LOG_ERROR("Failed to load texture file: {}", asset.mPath.string());
        return -1;
    }
    auto textureIndex =
        LoadTextureFromSurface(asset.mPath.stem().string(), surface.get());

    return textureIndex;
}

int CTextureManager::LoadTexture(const std::string& filename) {
    auto it = mLoadedTexturesIndices.find(filename);
    if (it != mLoadedTexturesIndices.end()) {
        return it->second;
    }
    auto textureAsset = mAssetParser.Get(EAssetType::Texture, filename);
    if (!textureAsset) {
        return -1;
    }
    return LoadTexture(textureAsset->get());
}

int CTextureManager::LoadTextureFromSurface(const std::string& filename,
                                            SDL_Surface* surface) {
    LoadedTexture tex = createTextureFromPixels(
        mContext.vmaAllocator, mContext.device,
        mBufferHandler.GetStagingBuffer(), surface->pixels,
        uint32_t(surface->w), uint32_t(surface->h), VK_FORMAT_R8G8B8A8_UNORM);
    int index = static_cast<int>(mLoadedTextures.size());
    mLoadedTextures.push_back(tex);
    mLoadedTexturesIndices[filename] = index;
    return index;
}

std::optional<LoadedTexture>
CTextureManager::GetTexture(const std::string& filename) {
    auto it = mLoadedTexturesIndices.find(filename);
    if (it != mLoadedTexturesIndices.end()) {
        return mLoadedTextures[it->second];
    }

    return std::nullopt;
}

std::optional<int>
CTextureManager::GetTextureIndex(const std::string& filename) const {
    auto it = mLoadedTexturesIndices.find(filename);
    if (it != mLoadedTexturesIndices.end()) {
        return it->second;
    }

    return std::nullopt;
}

const LoadedTexture& CTextureManager::GetTexture(int index) const {
    if (index < 0 || index >= static_cast<int>(mLoadedTextures.size())) {
        LOG_FATAL("Texture index out of bounds: %d", index);
    }
    return mLoadedTextures.at(index);
}

const std::unordered_map<std::string, int>&
CTextureManager::GetAllTextureIndices() const {
    return mLoadedTexturesIndices;
}

void CTextureManager::UnloadTexture(int index) {
    if (index < 0 || index >= static_cast<int>(mLoadedTextures.size())) {
        return;
    }

    LoadedTexture& texture = mLoadedTextures[index];

    vkDestroyImageView(mContext.device, texture.imageView, nullptr);
    vmaDestroyImage(mContext.vmaAllocator, texture.image, texture.allocation);
    auto it = std::find_if(
        mLoadedTexturesIndices.begin(), mLoadedTexturesIndices.end(),
        [index](const auto& pair) { return pair.second == index; });
    if (it != mLoadedTexturesIndices.end()) {
        // Erase the filename entry
        mLoadedTexturesIndices.erase(it);
    }

    mLoadedTextures[index] = LoadedTexture{}; // Mark as unloaded
}

} // namespace Renderer
