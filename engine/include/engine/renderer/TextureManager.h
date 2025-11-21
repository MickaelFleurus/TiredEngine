#pragma once
#include <string>
#include <unordered_map>

#include <vulkan/vulkan.h>

struct SDL_Surface;
namespace Utils {
class CFileHandler;
}

namespace Renderer {

struct VulkanTexture {
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView imageView = VK_NULL_HANDLE;
    VkSampler sampler = VK_NULL_HANDLE;
    uint32_t width = 0;
    uint32_t height = 0;
};

class CWindow;
class CTextureManager {
public:
    CTextureManager(const CWindow& window, Utils::CFileHandler& fileHandler);
    ~CTextureManager();

    VulkanTexture LoadTexture(const std::string& filename);
    VulkanTexture LoadTextureFromSurface(const std::string& filename,
                                         SDL_Surface* surface);
    VulkanTexture GetTexture(const std::string& filename);

private:
    const CWindow& mWindow;
    Utils::CFileHandler& mFileHandler;

    std::unordered_map<std::string, VulkanTexture> mLoadedTextures;
};
} // namespace Renderer