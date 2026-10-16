#include "engine/renderer/TextureManager.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <vector>

#include <SDL3/SDL_surface.h>

#include "engine/renderer/RendererUtils.h"
#include "engine/utils/AssetParser.h"
#include "engine/utils/FileHandler.h"
#include "engine/utils/Logger.h"
#include "engine/utils/StringId.h"
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

void TransitionImageLayout(Vulkan::CStagingBuffer& uploader, VkImage image,
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
    VkResult result = vmaCreateImage(allocator, &imageInfo, &allocInfo,
                                     &tex.image, &tex.allocation, nullptr);
    if (result != VK_SUCCESS) {
        LOG_FATAL(
            "Failed to create Vulkan image (VkResult {}, {}x{}, format {})",
            static_cast<int>(result), width, height, static_cast<int>(format));
    }

    // 1. Transition UNDEFINED -> TRANSFER_DST_OPTIMAL
    TransitionImageLayout(uploader, tex.image, format,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // 2. Copy pixel data via staging (extends your existing StagingUploader —
    // see below)
    uploader.UploadToImage(pixels, imageSize, tex.image, width, height);

    // 3. Transition TRANSFER_DST_OPTIMAL -> SHADER_READ_ONLY_OPTIMAL
    TransitionImageLayout(uploader, tex.image, format,
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

void registerBindlessTexture(const Vulkan::SContext& context, uint32_t index,
                             VkImageView view, VkSampler sampler) {
    VkDescriptorImageInfo imageInfo{
        .sampler = sampler,
        .imageView = view,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    VkWriteDescriptorSet write{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    write.dstSet = context.bindelessTextureSet;
    write.dstBinding = 0;
    write.dstArrayElement = index;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(context.device, 1, &write, 0, nullptr);
}

} // namespace

namespace Renderer {
CTextureManager::CTextureManager(const Vulkan::SContext& context,
                                 Vulkan::CSwapchain& swapchain,
                                 Vulkan::CVulkanRendering& renderer,
                                 Vulkan::CBufferHandler& bufferHandler,
                                 Utils::CFileHandler& fileHandler,
                                 const CAssetParser& assetParser)
    : mContext(context)
    , mSwapchain(swapchain)
    , mRenderer(renderer)
    , mBufferHandler(bufferHandler)
    , mFileHandler(fileHandler)
    , mAssetParser(assetParser)
    , mSampler(Renderer::CreateSampler(mContext.device)) {
    mLoadedTextures.reserve(Vulkan::kMaxTextures);
}

CTextureManager::~CTextureManager() {
    vkDestroySampler(mContext.device, mSampler, nullptr);
    for (auto& texture : mLoadedTextures) {
        vkDestroyImageView(mContext.device, texture.imageView, nullptr);
        vmaDestroyImage(mContext.vmaAllocator, texture.image,
                        texture.allocation);
    }
}

std::optional<std::pair<CStringId, uint64_t>>
CTextureManager::LoadTexture(const SAsset& asset) {
    std::unique_ptr<SDL_Surface, decltype(kGPUSurfaceDeleter)> surface{
        mFileHandler.LoadTextureFile(asset.mPath.string()), kGPUSurfaceDeleter};
    if (!surface) {
        LOG_ERROR("Failed to load texture file: {}", asset.mPath.string());
        return std::nullopt;
    }
    auto textureIndex =
        LoadTextureFromSurface(asset.mPath.stem().string(), surface.get());

    return textureIndex;
}

std::optional<std::pair<CStringId, uint64_t>>
CTextureManager::LoadTexture(const std::filesystem::path& filename) {
    std::unique_ptr<SDL_Surface, decltype(kGPUSurfaceDeleter)> surface{
        mFileHandler.LoadTextureFile(filename.string()), kGPUSurfaceDeleter};
    if (!surface) {
        LOG_ERROR("Failed to load texture file: {}", filename.string());
        return std::nullopt;
    }
    auto textureIndex =
        LoadTextureFromSurface(filename.stem().string(), surface.get());

    return textureIndex;
}

std::optional<std::pair<CStringId, uint64_t>>
CTextureManager::LoadTextureFromSurface(const std::string& filename,
                                        SDL_Surface* surface) {
    std::unique_ptr<SDL_Surface, decltype(kGPUSurfaceDeleter)> rgbaSurface{
        SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32),
        kGPUSurfaceDeleter};
    if (!rgbaSurface) {
        LOG_ERROR("Failed to convert texture to RGBA32: {}", filename);
        return std::nullopt;
    }

    if (SDL_MUSTLOCK(rgbaSurface.get()) &&
        !SDL_LockSurface(rgbaSurface.get())) {
        LOG_ERROR("Failed to lock converted texture surface: {}", filename);
        return std::nullopt;
    }

    const auto width = static_cast<uint32_t>(rgbaSurface->w);
    const auto height = static_cast<uint32_t>(rgbaSurface->h);
    const std::size_t rowSize = static_cast<std::size_t>(width) * 4;
    std::vector<uint8_t> rgbaPixels(rowSize * height);
    const auto* sourcePixels = static_cast<const uint8_t*>(rgbaSurface->pixels);
    for (uint32_t row = 0; row < height; ++row) {
        std::memcpy(rgbaPixels.data() + rowSize * row,
                    sourcePixels +
                        static_cast<std::size_t>(rgbaSurface->pitch) * row,
                    rowSize);
    }
    if (SDL_MUSTLOCK(rgbaSurface.get())) {
        SDL_UnlockSurface(rgbaSurface.get());
    }

    CStringId id{filename};
    LoadedTexture tex = createTextureFromPixels(
        mContext.vmaAllocator, mContext.device,
        mBufferHandler.GetStagingBuffer(), rgbaPixels.data(), width, height,
        VK_FORMAT_R8G8B8A8_UNORM);
    mBufferHandler.GetStagingBuffer().Flush();
    int index = static_cast<int>(mLoadedTextures.size());
    mLoadedTextures.push_back(tex);
    mLoadedTexturesIndices[id] = index;
    registerBindlessTexture(mContext, index, tex.imageView, mSampler);

    return std::make_pair(id, index);
}

std::optional<LoadedTexture> CTextureManager::GetTexture(CStringId filename) {
    auto it = mLoadedTexturesIndices.find(filename);
    if (it != mLoadedTexturesIndices.end()) {
        return mLoadedTextures[it->second];
    }

    return std::nullopt;
}

std::optional<uint64_t>
CTextureManager::GetTextureIndex(CStringId filename) const {
    auto it = mLoadedTexturesIndices.find(filename);
    if (it != mLoadedTexturesIndices.end()) {
        return it->second;
    }

    return std::nullopt;
}

const LoadedTexture& CTextureManager::GetTexture(uint64_t index) const {
    if (index < 0 || index >= static_cast<int>(mLoadedTextures.size())) {
        LOG_FATAL("Texture index out of bounds: %d", index);
    }
    return mLoadedTextures.at(index);
}

const std::unordered_map<CStringId, uint64_t, CStringIdHash>&
CTextureManager::GetAllTextureIndices() const {
    return mLoadedTexturesIndices;
}

void CTextureManager::UnloadTexture(uint64_t index) {
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
