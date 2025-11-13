#pragma once

#include "engine/material/AbstractMaterial.h"
#include "engine/renderer/PipelineTypes.h"
#include "engine/renderer/Vertex.h"
#include <concepts>
#include <cstdint>
#include <glm/mat4x4.hpp>
#include <ranges>
#include <span>
#include <vulkan/vulkan.h>

struct SDL_GPUBuffer;

namespace Material {
class AbstractMaterial;
}

namespace Renderer {

enum class ERenderLayer : uint8_t {
    Background = 0,
    World = 1,
    UI = 2,
    Debug = 3
};

struct SRenderable {
    glm::mat4 transform;
    Material::AbstractMaterial* material;
    ERenderLayer layer;
    float depth;

    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkBuffer indexBuffer = VK_NULL_HANDLE;
    uint32_t indexCount = 0;

    VkBuffer instanceBuffer = VK_NULL_HANDLE;
    uint32_t instanceCount = 0;

    uint64_t sortKey;

    void GenerateSortKey() {
        sortKey = (static_cast<uint64_t>(layer) << 56) |
                  (reinterpret_cast<uint64_t>(material->GetPipeline()) & 0xFFFF)
                      << 40 |
                  (reinterpret_cast<uint64_t>(material->GetTexture()) & 0xFFFF)
                      << 24 |
                  (static_cast<uint64_t>(depth * 1000.0f) & 0xFFFFFF);
    }

    bool operator<(const SRenderable& other) const {
        return sortKey < other.sortKey;
    }
};

struct VulkanBuffer {
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

struct VertexLayoutInfo {
    std::vector<VkVertexInputAttributeDescription> attributes;
    std::vector<VkVertexInputBindingDescription> bufferDescriptions;
};

template <typename T>
concept ContiguousRange = std::ranges::range<T> &&
                          std::contiguous_iterator<std::ranges::iterator_t<T>>;

VulkanBuffer CreateBuffer(VkDevice device, uint32_t size,
                          VkBufferUsageFlags bufferType,
                          VkPhysicalDeviceMemoryProperties memProperties);

void FillBuffer(VkDevice device, VulkanBuffer buffer, const void* data,
                uint32_t size);

VulkanBuffer
CreateAndFillBuffer(VkDevice device, const void* content, uint32_t size,
                    VkBufferUsageFlags bufferType,
                    VkPhysicalDeviceMemoryProperties memProperties);

template <ContiguousRange R>
VulkanBuffer
CreateAndFillVertexBuffer(VkDevice device, R& vertices,
                          VkPhysicalDeviceMemoryProperties memProperties) {
    uint32_t memSize = static_cast<uint32_t>(
        sizeof(std::ranges::range_value_t<R>) * std::ranges::size(vertices));
    return CreateAndFillBuffer(device, std::ranges::data(vertices), memSize,
                               VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                               memProperties);
}

template <ContiguousRange R>
VulkanBuffer
CreateAndFillIndexBuffer(VkDevice device, R& indices,
                         VkPhysicalDeviceMemoryProperties memProperties) {

    uint32_t memSize = static_cast<uint32_t>(
        sizeof(std::ranges::range_value_t<R>) * std::ranges::size(indices));
    return CreateAndFillBuffer(device, std::ranges::data(indices), memSize,
                               VK_BUFFER_USAGE_INDEX_BUFFER_BIT, memProperties);
}

VertexLayoutInfo CreateVertexLayout(Renderer::EVertexLayout layoutType);

} // namespace Renderer