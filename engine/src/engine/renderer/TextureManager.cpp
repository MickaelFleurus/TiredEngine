#include "engine/renderer/TextureManager.h"
#include "engine/renderer/RendererUtils.h"
#include "engine/renderer/VulkanRenderer.h"
#include "engine/renderer/Window.h"
#include "engine/utils/FileHandler.h"
#include <SDL3/SDL_surface.h>

namespace {
constexpr auto kGPUSurfaceDeleter = [](SDL_Surface* surface) {
    if (surface) {
        SDL_DestroySurface(surface);
    }
};
} // namespace

namespace Renderer {
CTextureManager::CTextureManager(const CWindow& window,
                                 Utils::CFileHandler& fileHandler)
    : mWindow(window), mFileHandler(fileHandler) {
}

CTextureManager::~CTextureManager() {
    VkDevice device = mWindow.GetVulkanRenderer().GetDevice();
    for (auto& pair : mLoadedTextures) {
        vkDestroyImageView(device, pair.second.imageView, nullptr);
        vkDestroySampler(device, pair.second.sampler, nullptr);
        vkDestroyImage(device, pair.second.image, nullptr);
        vkFreeMemory(device, pair.second.memory, nullptr);
    }
}

VulkanTexture CTextureManager::LoadTexture(const std::string& filename) {
    auto it = mLoadedTextures.find(filename);
    if (it != mLoadedTextures.end()) {
        return it->second;
    }

    std::unique_ptr<SDL_Surface, decltype(kGPUSurfaceDeleter)> surface{
        mFileHandler.LoadTextureFileBMP(filename), kGPUSurfaceDeleter};
    if (!surface) {
        return VulkanTexture{};
    }
    auto texture = LoadTextureFromSurface(filename, surface.get());

    return texture;
}

VulkanTexture
CTextureManager::LoadTextureFromSurface(const std::string& filename,
                                        SDL_Surface* surface) {
    if (!surface)
        return VulkanTexture{};

    VkDevice device = mWindow.GetVulkanRenderer().GetDevice();

    int width = surface->w;
    int height = surface->h;
    void* pixels = surface->pixels;
    size_t imageSize = width * height * 4; // Assuming SDL_PIXELFORMAT_RGBA32

    // 1. Create staging buffer and copy pixel data
    Renderer::VulkanBuffer buffer = Renderer::CreateBuffer(
        device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        mWindow.GetVulkanRenderer().GetVulkanPhysicalDevice().memoryProperties);

    void* data;
    vkMapMemory(device, buffer.memory, 0, imageSize, 0, &data);
    memcpy(data, pixels, imageSize);
    vkUnmapMemory(device, buffer.memory);

    // 2. Create Vulkan image
    VulkanImage image = CreateImage(
        device, width, height, VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        mWindow.GetVulkanRenderer().GetVulkanPhysicalDevice().memoryProperties,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // 3. Transition image layout and copy buffer to image
    TransitionImageLayout(
        image.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mWindow.GetVulkanRenderer());
    CopyBufferToImage(buffer.buffer, image.image, width, height,
                      mWindow.GetVulkanRenderer());
    TransitionImageLayout(image.image, VK_FORMAT_R8G8B8A8_UNORM,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                          mWindow.GetVulkanRenderer());

    // 4. Create image view and sampler
    VkImageView imageView;
    CreateImageView(device, image.image, VK_FORMAT_R8G8B8A8_UNORM,
                    VK_IMAGE_ASPECT_COLOR_BIT, imageView);
    VkSampler sampler = CreateSampler(device);

    VulkanTexture texture{image.image, image.memory, imageView, sampler};
    mLoadedTextures[filename] = texture;
    return texture;
}

VulkanTexture CTextureManager::GetTexture(const std::string& filename) {
    auto it = mLoadedTextures.find(filename);
    if (it != mLoadedTextures.end()) {
        return it->second;
    }

    return VulkanTexture{};
}

} // namespace Renderer