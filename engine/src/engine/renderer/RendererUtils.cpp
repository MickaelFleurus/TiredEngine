#include "engine/renderer/RendererUtils.h"
#include "engine/utils/Logger.h"

namespace {
uint32_t FindMemoryType(uint32_t typeFilter,
                        VkPhysicalDeviceMemoryProperties memProperties,
                        VkMemoryPropertyFlags properties) {

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) ==
                properties) {
            return i;
        }
    }

    LOG_FATAL("Failed to find suitable memory type!");
    return 0;
}
} // namespace

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

    // Buffer 1: Instance data
    VkVertexInputAttributeDescription attr2{}; // instancePosition
    attr2.location = 2;
    attr2.binding = 1;
    attr2.format = VK_FORMAT_R32G32_SFLOAT;
    attr2.offset = 0;

    VkVertexInputAttributeDescription attr3{}; // instanceSize
    attr3.location = 3;
    attr3.binding = 1;
    attr3.format = VK_FORMAT_R32G32_SFLOAT;
    attr3.offset = sizeof(float) * 2;

    VkVertexInputAttributeDescription attr4{}; // instanceUVRect
    attr4.location = 4;
    attr4.binding = 1;
    attr4.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attr4.offset = sizeof(float) * 4;

    VkVertexInputAttributeDescription attr5{}; // instanceColor
    attr5.location = 5;
    attr5.binding = 1;
    attr5.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attr5.offset = sizeof(float) * 8;

    info.attributes = {attr0, attr1, attr2, attr3, attr4, attr5};

    VkVertexInputBindingDescription vertexBufferDesc{};
    vertexBufferDesc.binding = 0;
    vertexBufferDesc.stride = sizeof(float) * 5;
    vertexBufferDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputBindingDescription instanceBufferDesc{};
    instanceBufferDesc.binding = 1;
    instanceBufferDesc.stride = sizeof(float) * 12; // vec2 + vec2 + vec4 + vec4
    instanceBufferDesc.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    info.bufferDescriptions = {vertexBufferDesc, instanceBufferDesc};

    return info;
}

Renderer::VertexLayoutInfo CreateMeshVertexLayout() {
    Renderer::VertexLayoutInfo info;

    // Buffer 0: Position + Normal + TexCoord (for 3D meshes)
    VkVertexInputAttributeDescription attr0{};
    attr0.location = 0;
    attr0.binding = 0;
    attr0.format = VK_FORMAT_R32G32B32_SFLOAT;
    attr0.offset = 0;

    VkVertexInputAttributeDescription attr1{}; // Normal
    attr1.location = 1;
    attr1.binding = 0;
    attr1.format = VK_FORMAT_R32G32B32_SFLOAT;
    attr1.offset = sizeof(float) * 3;

    VkVertexInputAttributeDescription attr2{}; // TexCoord
    attr2.location = 2;
    attr2.binding = 0;
    attr2.format = VK_FORMAT_R32G32_SFLOAT;
    attr2.offset = sizeof(float) * 6;

    info.attributes = {attr0, attr1, attr2};

    VkVertexInputBindingDescription bufferDesc{};
    bufferDesc.binding = 0;
    bufferDesc.stride = sizeof(float) * 8; // vec3 + vec3 + vec2
    bufferDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    info.bufferDescriptions = {bufferDesc};

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

VulkanBuffer CreateBuffer(VkDevice device, uint32_t size,
                          VkBufferUsageFlags bufferType,
                          VkPhysicalDeviceMemoryProperties memProperties) {

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.usage = bufferType;
    bufferInfo.size = size;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer buffer;
    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        LOG_ERROR("Failed to create buffer!");
        return {};
    }
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex =
        FindMemoryType(memRequirements.memoryTypeBits, memProperties,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VkDeviceMemory vertexBufferMemory;
    if (vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory) !=
        VK_SUCCESS) {
        LOG_ERROR("Failed to allocate vertex buffer memory!");
        return {};
    }
    vkBindBufferMemory(device, buffer, vertexBufferMemory, 0);

    return {device, buffer, vertexBufferMemory};
}

void FillBuffer(VkDevice device, VulkanBuffer& buffer, const void* data,
                uint32_t size) {
    void* mappedData;
    vkMapMemory(device, buffer.memory, 0, size, 0, &mappedData);
    memcpy(mappedData, data, size);
    VkMappedMemoryRange range{};
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = buffer.memory;
    range.offset = 0;
    range.size = size;
    vkFlushMappedMemoryRanges(device, 1, &range);
    vkUnmapMemory(device, buffer.memory);
}

VulkanBuffer
CreateAndFillBuffer(VkDevice device, const void* content, uint32_t size,
                    VkBufferUsageFlags bufferType,
                    VkPhysicalDeviceMemoryProperties memProperties) {

    VulkanBuffer buffer = CreateBuffer(device, size, bufferType, memProperties);
    FillBuffer(device, buffer, content, size);
    return std::move(buffer);
}

} // namespace Renderer