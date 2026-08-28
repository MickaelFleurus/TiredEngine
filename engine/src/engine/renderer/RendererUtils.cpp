#include "engine/renderer/RendererUtils.h"

#include "engine/utils/Logger.h"

namespace Renderer {

VkSampler CreateSampler(VkDevice device) {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    VkSampler sampler;
    if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) !=
        VK_SUCCESS) {
        LOG_FATAL("Failed to create texture sampler!");
    }
    return sampler;
}

VkFormat SDLPixelFormatToVulkanFormat(SDL_PixelFormat sdlFormat) {
    // Array formats (little-endian on little-endian systems)
    if (sdlFormat == SDL_PIXELFORMAT_RGB24) {
        return VK_FORMAT_R8G8B8_UNORM;
    }
    if (sdlFormat == SDL_PIXELFORMAT_BGR24) {
        return VK_FORMAT_B8G8R8_UNORM;
    }

    // 32-bit RGBA formats
    if (sdlFormat == SDL_PIXELFORMAT_RGBA8888 ||
        sdlFormat == SDL_PIXELFORMAT_RGBA32) {
        return VK_FORMAT_R8G8B8A8_UNORM;
    }
    if (sdlFormat == SDL_PIXELFORMAT_ARGB8888 ||
        sdlFormat == SDL_PIXELFORMAT_ARGB32) {
        return VK_FORMAT_A8B8G8R8_UNORM_PACK32;
    }
    if (sdlFormat == SDL_PIXELFORMAT_BGRA8888 ||
        sdlFormat == SDL_PIXELFORMAT_BGRA32) {
        return VK_FORMAT_B8G8R8A8_UNORM;
    }
    if (sdlFormat == SDL_PIXELFORMAT_ABGR8888 ||
        sdlFormat == SDL_PIXELFORMAT_ABGR32) {
        return VK_FORMAT_A8B8G8R8_UNORM_PACK32;
    }

    // 32-bit RGB formats (no alpha)
    if (sdlFormat == SDL_PIXELFORMAT_XRGB8888 ||
        sdlFormat == SDL_PIXELFORMAT_XRGB32) {
        return VK_FORMAT_R8G8B8A8_UNORM; // Pad X with A
    }
    if (sdlFormat == SDL_PIXELFORMAT_RGBX8888 ||
        sdlFormat == SDL_PIXELFORMAT_RGBX32) {
        return VK_FORMAT_R8G8B8A8_UNORM; // Pad X with A
    }

    if (sdlFormat == SDL_PIXELFORMAT_XBGR8888 ||
        sdlFormat == SDL_PIXELFORMAT_XBGR32) {
        return VK_FORMAT_B8G8R8A8_UNORM; // Pad X with A
    }
    if (sdlFormat == SDL_PIXELFORMAT_BGRX8888 ||
        sdlFormat == SDL_PIXELFORMAT_BGRX32) {
        return VK_FORMAT_B8G8R8A8_UNORM; // Pad X with A
    }

    // 16-bit RGB formats
    if (sdlFormat == SDL_PIXELFORMAT_RGB565) {
        return VK_FORMAT_R5G6B5_UNORM_PACK16;
    }
    if (sdlFormat == SDL_PIXELFORMAT_BGR565) {
        return VK_FORMAT_B5G6R5_UNORM_PACK16;
    }

    // 16-bit RGBA formats
    if (sdlFormat == SDL_PIXELFORMAT_RGBA4444) {
        return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
    }
    if (sdlFormat == SDL_PIXELFORMAT_BGRA4444) {
        return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
    }
    if (sdlFormat == SDL_PIXELFORMAT_RGBA5551) {
        return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
    }
    if (sdlFormat == SDL_PIXELFORMAT_BGRA5551) {
        return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
    }

    // 8-bit grayscale (stored as alpha only)
    if (sdlFormat == SDL_PIXELFORMAT_INDEX8) {
        // Indexed format needs palette conversion - not directly supported
        return VK_FORMAT_UNDEFINED;
    }

    // Unsupported format
    return VK_FORMAT_UNDEFINED;
}

} // namespace Renderer
