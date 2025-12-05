#include "engine/renderer/RendererUtils.h"

#include "engine/core/DataTypes.h"
#include "engine/renderer/MemoryAllocator.h"
#include "engine/utils/Logger.h"
#include "engine/vulkan/VulkanContext.h"
#include "engine/vulkan/VulkanRendering.h"

namespace {
Renderer::VertexLayoutInfo CreateSimpleVertexLayout() {
    Renderer::VertexLayoutInfo info;

    // Buffer 0: Position + TexCoord (for basic sprites/quads)
    VkVertexInputAttributeDescription attr0{};
    attr0.binding = 0;
    attr0.format = VK_FORMAT_R32G32B32_SFLOAT;
    attr0.location = 0;
    attr0.offset = 0;

    VkVertexInputAttributeDescription attr1{};
    attr1.binding = 0;
    attr1.format = VK_FORMAT_R32G32_SFLOAT;
    attr1.location = 1;
    attr1.offset = sizeof(float) * 3;

    info.attributes = {attr0, attr1};

    VkVertexInputBindingDescription bufferDesc{};
    bufferDesc.binding = 0;
    bufferDesc.stride = sizeof(float) * 5; // vec3 + vec2
    bufferDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    info.bufferDescriptions = {bufferDesc};

    return info;
}

Renderer::VertexLayoutInfo CreateInstancedVertexLayout() {
    Renderer::VertexLayoutInfo info;

    // Buffer 0: Position + TexCoord (per-vertex)
    VkVertexInputAttributeDescription vertexPosition{};
    vertexPosition.binding = 0;
    vertexPosition.format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexPosition.location = 0;
    vertexPosition.offset = 0;

    VkVertexInputAttributeDescription vertexUV{};
    vertexUV.binding = 0;
    vertexUV.format = VK_FORMAT_R32G32_SFLOAT;
    vertexUV.location = 1;
    vertexUV.offset = sizeof(float) * 3;

    // Buffer 1: Instance data
    // Model matrix (4 vec4)
    VkVertexInputAttributeDescription modelMatrixPart0{};
    modelMatrixPart0.location = 2;
    modelMatrixPart0.binding = 1;
    modelMatrixPart0.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    modelMatrixPart0.offset = 0;

    VkVertexInputAttributeDescription modelMatrixPart1{};
    modelMatrixPart1.location = 3;
    modelMatrixPart1.binding = 1;
    modelMatrixPart1.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    modelMatrixPart1.offset = sizeof(float) * 4;

    VkVertexInputAttributeDescription modelMatrixPart2{};
    modelMatrixPart2.location = 4;
    modelMatrixPart2.binding = 1;
    modelMatrixPart2.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    modelMatrixPart2.offset = sizeof(float) * 8;

    VkVertexInputAttributeDescription modelMatrixPart3{};
    modelMatrixPart3.location = 5;
    modelMatrixPart3.binding = 1;
    modelMatrixPart3.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    modelMatrixPart3.offset = sizeof(float) * 12;

    VkVertexInputAttributeDescription color{};
    color.location = 6;
    color.binding = 1;
    color.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    color.offset = sizeof(float) * 16;

    VkVertexInputAttributeDescription materialId{};
    materialId.location = 7;
    materialId.binding = 1;
    materialId.format = VK_FORMAT_R32_UINT;
    materialId.offset = sizeof(float) * 20;

    VkVertexInputAttributeDescription texId{};
    texId.location = 8;
    texId.binding = 1;
    texId.format = VK_FORMAT_R32_UINT;
    texId.offset = sizeof(float) * 20 + sizeof(uint32_t);

    info.attributes = {vertexPosition,
                       vertexUV,
                       modelMatrixPart0,
                       modelMatrixPart1,
                       modelMatrixPart2,
                       modelMatrixPart3,
                       color,
                       materialId,
                       texId};

    VkVertexInputBindingDescription vertexBufferDesc{};
    vertexBufferDesc.binding = 0;
    vertexBufferDesc.stride = sizeof(float) * 5;
    vertexBufferDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputBindingDescription instanceBufferDesc{};
    instanceBufferDesc.binding = 1;
    instanceBufferDesc.stride =
        sizeof(float) * 20 + sizeof(uint32_t) * 2; // 4x vec4 + vec4 + 2 ushorts
    instanceBufferDesc.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    info.bufferDescriptions = {vertexBufferDesc, instanceBufferDesc};

    return info;
}

Renderer::VertexLayoutInfo CreateMeshVertexLayout() {
    Renderer::VertexLayoutInfo info;

    // Buffer 0: Position + TexCoord (per-vertex)
    VkVertexInputAttributeDescription vertexPosition{};
    vertexPosition.binding = 0;
    vertexPosition.format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexPosition.location = 0;
    vertexPosition.offset = 0;

    VkVertexInputAttributeDescription vertexUV{};
    vertexUV.binding = 0;
    vertexUV.format = VK_FORMAT_R32G32_SFLOAT;
    vertexUV.location = 1;
    vertexUV.offset = sizeof(float) * 4;

    VkVertexInputAttributeDescription vertexNormal{};
    vertexNormal.binding = 0;
    vertexNormal.format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexNormal.location = 2;
    vertexNormal.offset = sizeof(float) * 8;

    // Buffer 1: Instance data
    // Model matrix (4 vec4)
    VkVertexInputAttributeDescription modelMatrixPart0{};
    modelMatrixPart0.location = 3;
    modelMatrixPart0.binding = 1;
    modelMatrixPart0.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    modelMatrixPart0.offset = 0;

    VkVertexInputAttributeDescription modelMatrixPart1{};
    modelMatrixPart1.location = 4;
    modelMatrixPart1.binding = 1;
    modelMatrixPart1.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    modelMatrixPart1.offset = sizeof(float) * 4;

    VkVertexInputAttributeDescription modelMatrixPart2{};
    modelMatrixPart2.location = 5;
    modelMatrixPart2.binding = 1;
    modelMatrixPart2.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    modelMatrixPart2.offset = sizeof(float) * 8;

    VkVertexInputAttributeDescription modelMatrixPart3{};
    modelMatrixPart3.location = 6;
    modelMatrixPart3.binding = 1;
    modelMatrixPart3.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    modelMatrixPart3.offset = sizeof(float) * 12;

    VkVertexInputAttributeDescription color{};
    color.location = 7;
    color.binding = 1;
    color.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    color.offset = sizeof(float) * 16;

    VkVertexInputAttributeDescription materialId{};
    materialId.location = 8;
    materialId.binding = 1;
    materialId.format = VK_FORMAT_R32_UINT;
    materialId.offset = sizeof(float) * 20;

    VkVertexInputAttributeDescription texId{};
    texId.location = 9;
    texId.binding = 1;
    texId.format = VK_FORMAT_R32_UINT;
    texId.offset = sizeof(float) * 20 + sizeof(uint32_t);

    info.attributes = {vertexPosition,
                       vertexUV,
                       vertexNormal,
                       modelMatrixPart0,
                       modelMatrixPart1,
                       modelMatrixPart2,
                       modelMatrixPart3,
                       color,
                       texId,
                       materialId};

    VkVertexInputBindingDescription vertexBufferDesc{};
    vertexBufferDesc.binding = 0;
    vertexBufferDesc.stride = sizeof(Core::SVertex);
    vertexBufferDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputBindingDescription instanceBufferDesc{};
    instanceBufferDesc.binding = 1;
    instanceBufferDesc.stride = sizeof(Core::SInstanceData);
    instanceBufferDesc.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    info.bufferDescriptions = {vertexBufferDesc, instanceBufferDesc};

    return info;
}

} // namespace

namespace Renderer {
VertexLayoutInfo CreateVertexLayout(Renderer::EVertexLayout layoutType) {
    switch (layoutType) {
    case Renderer::EVertexLayout::Simple:
        return CreateSimpleVertexLayout();
    case Renderer::EVertexLayout::Instanced:
        return CreateInstancedVertexLayout();
    case Renderer::EVertexLayout::Mesh3D:
        return CreateMeshVertexLayout();
    default:
        return CreateSimpleVertexLayout();
    }
}

VkCommandBuffer BeginSingleTimeCommands(const Vulkan::CVulkanContext& context) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = context.GetCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    if (vkAllocateCommandBuffers(context.GetDevice(), &allocInfo,
                                 &commandBuffer) != VK_SUCCESS) {
        LOG_ERROR("Failed to allocate command buffer!");
        return VK_NULL_HANDLE;
    }

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void EndSingleTimeCommands(const Vulkan::CVulkanContext& context,
                           const Vulkan::CVulkanRendering& renderer,
                           VkCommandBuffer commandBuffer) {

    vkEndCommandBuffer(commandBuffer);
    renderer.SubmitSyncSingleUse(commandBuffer);

    vkFreeCommandBuffers(context.GetDevice(), context.GetCommandPool(), 1,
                         &commandBuffer);
}

VulkanImage CreateImage(const Vulkan::CVulkanContext& context,
                        Renderer::CMemoryAllocator& allocator, uint32_t width,
                        uint32_t height, VkFormat format, VkImageTiling tiling,
                        VkImageUsageFlags usage,
                        VkMemoryPropertyFlags properties) {

    VkDevice device = context.GetDevice();
    VkImage image;
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        LOG_ERROR("Failed to create image!");
        return {};
    }

    VkDeviceMemory memory = allocator.AllocateMemory(image, properties);

    vkBindImageMemory(device, image, memory, 0);
    return {image, memory};
}

void TransitionImageLayout(VkImage image, VkFormat format,
                           VkImageLayout oldLayout, VkImageLayout newLayout,
                           const Vulkan::CVulkanContext& context,
                           Vulkan::CVulkanRendering& renderer) {
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(context);
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    // Set access masks and pipeline stages based on layouts
    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        LOG_ERROR("Unsupported layout transition!");
        EndSingleTimeCommands(context, renderer, commandBuffer);
        return;
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0,
                         nullptr, 0, nullptr, 1, &barrier);

    EndSingleTimeCommands(context, renderer, commandBuffer);
}

void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                       uint32_t height, const Vulkan::CVulkanContext& context,
                       Vulkan::CVulkanRendering& renderer) {
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(context);
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
    EndSingleTimeCommands(context, renderer, commandBuffer);
}

void CreateImageView(VkDevice device, VkImage image, VkFormat format,
                     VkImageAspectFlags aspectFlags, VkImageView& imageView) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) !=
        VK_SUCCESS) {
        LOG_FATAL("Failed to create image view!");
    }
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

} // namespace Renderer