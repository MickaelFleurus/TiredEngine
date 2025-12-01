#include "engine/renderer/TextureManager.h"
#include "engine/renderer/RendererUtils.h"

#include "engine/utils/FileHandler.h"
#include "engine/vulkan/BufferHandler.h"
#include "engine/vulkan/Constants.h"
#include "engine/vulkan/DescriptorStorage.h"
#include "engine/vulkan/VulkanContext.h"
#include "engine/vulkan/VulkanRendering.h"
#include <SDL3/SDL_surface.h>

namespace {
constexpr auto kGPUSurfaceDeleter = [](SDL_Surface* surface) {
    if (surface) {
        SDL_DestroySurface(surface);
    }
};

} // namespace

namespace Renderer {
CTextureManager::CTextureManager(const Vulkan::CVulkanContext& context,
                                 Vulkan::CVulkanRendering& renderer,
                                 Renderer::CMemoryAllocator& memoryAllocator,
                                 Vulkan::CBufferHandler& bufferHandler,
                                 Utils::CFileHandler& fileHandler,
                                 Vulkan::CDescriptorStorage& descriptorStorage)
    : mContext(context)
    , mRenderer(renderer)
    , mMemoryAllocator(memoryAllocator)
    , mBufferHandler(bufferHandler)
    , mFileHandler(fileHandler)
    , mDescriptorStorage(descriptorStorage) {
    mLoadedTextures.reserve(Vulkan::kMaxTextures);
}

CTextureManager::~CTextureManager() {
    VkDevice device = mContext.GetDevice();
    for (auto& texture : mLoadedTextures) {
        vkDestroyImageView(device, texture.imageView, nullptr);
        vkDestroySampler(device, texture.sampler, nullptr);
        vkDestroyImage(device, texture.image, nullptr);
        vkFreeMemory(device, texture.memory, nullptr);
    }
}

int CTextureManager::LoadTexture(const std::string& filename) {
    auto it = mLoadedTexturesIndices.find(filename);
    if (it != mLoadedTexturesIndices.end()) {
        return it->second;
    }

    std::unique_ptr<SDL_Surface, decltype(kGPUSurfaceDeleter)> surface{
        mFileHandler.LoadTextureFileBMP(filename), kGPUSurfaceDeleter};
    if (!surface) {
        return -1;
    }
    auto textureIndex = LoadTextureFromSurface(filename, surface.get());

    return textureIndex;
}

int CTextureManager::LoadTextureFromSurface(const std::string& filename,
                                            SDL_Surface* surface) {
    VkDevice device = mContext.GetDevice();

    int width = surface->w;
    int height = surface->h;
    void* pixels = surface->pixels;
    int imageSize =
        width * height * sizeof(float); // Assuming SDL_PIXELFORMAT_RGBA32

    auto bufferHandle = mBufferHandler.CreateBuffer(
        sizeof(float), imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

    auto range = bufferHandle->Reserve(static_cast<int>(imageSize));
    bufferHandle->Fill(pixels, static_cast<int>(imageSize), 0, range.value());

    // 2. Create Vulkan image
    VulkanImage image = CreateImage(
        mContext, mMemoryAllocator, width, height, VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // 3. Transition image layout and copy buffer to image
    TransitionImageLayout(
        image.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mContext, mRenderer);
    CopyBufferToImage(bufferHandle->GetBuffer(), image.image, width, height,
                      mContext, mRenderer);
    TransitionImageLayout(image.image, VK_FORMAT_R8G8B8A8_UNORM,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mContext,
                          mRenderer);

    // 4. Create image view and sampler
    VkImageView imageView;
    CreateImageView(device, image.image, VK_FORMAT_R8G8B8A8_UNORM,
                    VK_IMAGE_ASPECT_COLOR_BIT, imageView);
    VkSampler sampler = CreateSampler(device);

    VulkanTexture texture{image.image, image.memory, imageView, sampler};
    int index = static_cast<int>(mLoadedTextures.size());
    mLoadedTextures.push_back(texture);
    mLoadedTexturesIndices[filename] = index;
    UpdateDescriptor(index);
    return index;
}

std::optional<VulkanTexture>
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

void CTextureManager::UpdateDescriptor(int index) {

    VkDescriptorSet descriptorSet = mDescriptorStorage.GetDescriptorSet();

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = mLoadedTextures[index].imageView;
    imageInfo.sampler = mLoadedTextures[index].sampler;

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = descriptorSet;
    write.dstBinding = Vulkan::kTextureBinding;
    write.dstArrayElement = static_cast<uint32_t>(index);
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.descriptorCount = 1;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(mContext.GetDevice(), 1, &write, 0, nullptr);
}

} // namespace Renderer