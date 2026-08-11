#include "engine/renderer/RendererUtils.h"

#include "engine/renderer/MemoryAllocator.h"
#include "engine/utils/Logger.h"
#include "engine/vulkan/VulkanContext.h"
#include "engine/vulkan/VulkanRendering.h"

namespace {} // namespace

namespace Renderer {

VkCommandBuffer BeginSingleTimeCommands(const Vulkan::SContext& context,
                                        Vulkan::CSwapchain& swapchain) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = swapchain.GetCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    if (vkAllocateCommandBuffers(context.device, &allocInfo, &commandBuffer) !=
        VK_SUCCESS) {
        LOG_ERROR("Failed to allocate command buffer!");
        return VK_NULL_HANDLE;
    }

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void EndSingleTimeCommands(const Vulkan::SContext& context,
                           Vulkan::CSwapchain& swapchain,
                           const Vulkan::CVulkanRendering& renderer,
                           VkCommandBuffer commandBuffer) {

    vkEndCommandBuffer(commandBuffer);
    renderer.SubmitSyncSingleUse(commandBuffer);

    vkFreeCommandBuffers(context.device, swapchain.GetCommandPool(), 1,
                         &commandBuffer);
}

void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                       uint32_t height, const Vulkan::SContext& context,
                       Vulkan::CSwapchain& swapchain,
                       Vulkan::CVulkanRendering& renderer) {
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(context, swapchain);
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;   // Tightly packed
    region.bufferImageHeight = 0; // Tightly packed

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(commandBuffer, buffer, image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    EndSingleTimeCommands(context, swapchain, renderer, commandBuffer);
}

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
