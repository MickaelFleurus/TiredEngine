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
class IVulkanContextGetter;
class CVulkanRendering;
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

class CBufferHandler;
class CTextureManager {
public:
    CTextureManager(const Vulkan::IVulkanContextGetter& context,
                    Vulkan::CVulkanRendering& renderer,
                    CBufferHandler& bufferHandler,
                    Utils::CFileHandler& fileHandler);
    ~CTextureManager();

    int LoadTexture(const std::string& filename);
    int LoadTextureFromSurface(const std::string& filename,
                               SDL_Surface* surface);
    std::optional<VulkanTexture> GetTexture(const std::string& filename);
    std::optional<int> GetTextureIndex(const std::string& filename) const;

private:
    void UpdateDescriptor(int index);

    const Vulkan::IVulkanContextGetter& mContext;
    Vulkan::CVulkanRendering& mRenderer;

    CBufferHandler& mBufferHandler;
    Utils::CFileHandler& mFileHandler;

    std::vector<VulkanTexture> mLoadedTextures;
    std::unordered_map<std::string, int> mLoadedTexturesIndices;

    VkDescriptorPool mTextureDescriptorPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout mTexturesSetLayout = VK_NULL_HANDLE;
};
} // namespace Renderer