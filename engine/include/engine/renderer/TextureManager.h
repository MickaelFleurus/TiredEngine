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
class CVulkanContext;
class CVulkanRendering;
class CDescriptorStorage;
class CBufferHandler;
} // namespace Vulkan

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
    CTextureManager(const Vulkan::CVulkanContext& context,
                    Vulkan::CVulkanRendering& renderer,
                    Renderer::CMemoryAllocator& memoryAllocator,
                    Vulkan::CBufferHandler& bufferHandler,
                    Utils::CFileHandler& fileHandler,
                    Vulkan::CDescriptorStorage& descriptorStorage);
    ~CTextureManager();

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

    const Vulkan::CVulkanContext& mContext;
    Vulkan::CVulkanRendering& mRenderer;
    Renderer::CMemoryAllocator& mMemoryAllocator;

    Vulkan::CBufferHandler& mBufferHandler;
    Utils::CFileHandler& mFileHandler;
    Vulkan::CDescriptorStorage& mDescriptorStorage;

    std::vector<VulkanTexture> mLoadedTextures;
    std::unordered_map<std::string, int> mLoadedTexturesIndices;
};
} // namespace Renderer