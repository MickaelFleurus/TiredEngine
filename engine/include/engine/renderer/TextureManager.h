#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "engine/utils/StringId.h"

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

class CTextureManager {
public:
    CTextureManager(const Vulkan::SContext& context,
                    Vulkan::CSwapchain& swapchain,
                    Vulkan::CVulkanRendering& renderer,
                    Vulkan::CBufferHandler& bufferHandler,
                    Utils::CFileHandler& fileHandler,
                    const CAssetParser& assetParser);
    ~CTextureManager();

    std::optional<std::pair<CStringId, uint64_t>>
    LoadTexture(const SAsset& asset);

    std::optional<std::pair<CStringId, uint64_t>>
    LoadTexture(const std::filesystem::path& filename);

    std::optional<std::pair<CStringId, uint64_t>>
    LoadTextureFromSurface(const std::string& filename, SDL_Surface* surface);

    std::optional<LoadedTexture> GetTexture(CStringId filename);
    std::optional<uint64_t> GetTextureIndex(CStringId filename) const;

    void UnloadTexture(uint64_t index);

    // ONLY IMGUI
    const LoadedTexture& GetTexture(uint64_t index) const;
    const std::unordered_map<CStringId, uint64_t, CStringIdHash>&
    GetAllTextureIndices() const;

private:
    const Vulkan::SContext& mContext;
    Vulkan::CSwapchain& mSwapchain;
    Vulkan::CVulkanRendering& mRenderer;

    Vulkan::CBufferHandler& mBufferHandler;
    Utils::CFileHandler& mFileHandler;
    const CAssetParser& mAssetParser;

    std::vector<LoadedTexture> mLoadedTextures;
    std::unordered_map<CStringId, uint64_t, CStringIdHash>
        mLoadedTexturesIndices;
};
} // namespace Renderer
