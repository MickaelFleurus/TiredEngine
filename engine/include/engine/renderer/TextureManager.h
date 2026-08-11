#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

struct SDL_Surface;

namespace Utils {
class CFileHandler;
}

namespace Vulkan {
struct SContext;
class CSwapchain;
class CVulkanRendering;
class CBufferHandler;
} // namespace Vulkan

class CAssetParser;
struct SAsset;

namespace Renderer {

struct LoadedTexture {
    VkImage image = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    VkImageView imageView = VK_NULL_HANDLE;
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
                    const CAssetParser& assetParser);
    ~CTextureManager();

    int LoadTexture(const SAsset& asset);
    int LoadTexture(const std::string& filename);
    int LoadTextureFromSurface(const std::string& filename,
                               SDL_Surface* surface);
    std::optional<LoadedTexture> GetTexture(const std::string& filename);
    std::optional<int> GetTextureIndex(const std::string& filename) const;

    void UnloadTexture(int index);

    // ONLY IMGUI
    const LoadedTexture& GetTexture(int index) const;
    const std::unordered_map<std::string, int>& GetAllTextureIndices() const;

private:
    const Vulkan::SContext& mContext;
    Vulkan::CSwapchain& mSwapchain;
    Vulkan::CVulkanRendering& mRenderer;
    Renderer::CMemoryAllocator& mMemoryAllocator;

    Vulkan::CBufferHandler& mBufferHandler;
    Utils::CFileHandler& mFileHandler;
    const CAssetParser& mAssetParser;

    std::vector<LoadedTexture> mLoadedTextures;
    std::unordered_map<std::string, int> mLoadedTexturesIndices;
};
} // namespace Renderer
