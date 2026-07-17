#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <vulkan/vulkan.h>

struct SDL_Surface;

namespace Utils {
class CFileHandler;
}

namespace Vulkan {
struct SContext;
class CSwapchain;
class CVulkanRendering;
class CDescriptorStorage;
class CBufferHandler;
} // namespace Vulkan

class CAssetParser;
struct SAsset;

namespace Renderer {

struct VulkanTexture {
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView imageView = VK_NULL_HANDLE;
    VkSampler sampler = VK_NULL_HANDLE;
    uint32_t width = 0;
    uint32_t height = 0;
};

class CMemoryAllocator;
class CTextureManager {
public:
    CTextureManager(const Vulkan::SContext& context,
                    Vulkan::CSwapchain& swapchain,
                    Vulkan::CVulkanRendering& renderer,
                    Renderer::CMemoryAllocator& memoryAllocator,
                    Vulkan::CBufferHandler& bufferHandler,
                    Utils::CFileHandler& fileHandler,
                    Vulkan::CDescriptorStorage& descriptorStorage,
                    const CAssetParser& assetParser);
    ~CTextureManager();

    int LoadTexture(const SAsset& asset);
    int LoadTexture(const std::string& filename);
    int LoadTextureFromSurface(const std::string& filename,
                               SDL_Surface* surface);
    std::optional<VulkanTexture> GetTexture(const std::string& filename);
    std::optional<int> GetTextureIndex(const std::string& filename) const;

    const VulkanTexture& GetTexture(int index) const;
    const std::unordered_map<std::string, int>& GetAllTextureIndices() const;

    void UnloadTexture(int index);

private:
    void UpdateDescriptor(int index);

    const Vulkan::SContext& mContext;
    Vulkan::CSwapchain& mSwapchain;
    Vulkan::CVulkanRendering& mRenderer;
    Renderer::CMemoryAllocator& mMemoryAllocator;

    Vulkan::CBufferHandler& mBufferHandler;
    Utils::CFileHandler& mFileHandler;
    Vulkan::CDescriptorStorage& mDescriptorStorage;
    const CAssetParser& mAssetParser;

    std::vector<VulkanTexture> mLoadedTextures;
    std::unordered_map<std::string, int> mLoadedTexturesIndices;
};
} // namespace Renderer
