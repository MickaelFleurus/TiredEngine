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

namespace Vulkan {
class IVulkanContextGetter;
class CVulkanRendering;
} // namespace Vulkan

namespace Material {
class AbstractMaterial;
}

namespace Renderer {
class CMemoryAllocator;

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
                  (static_cast<uint64_t>(material->GetTextureIndex()) & 0xFFFF)
                      << 24 |
                  (static_cast<uint64_t>(depth * 1000.0f) & 0xFFFFFF);
    }

    bool operator<(const SRenderable& other) const {
        return sortKey < other.sortKey;
    }
};

struct VulkanImage {
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

struct VertexLayoutInfo {
    std::vector<VkVertexInputAttributeDescription> attributes;
    std::vector<VkVertexInputBindingDescription> bufferDescriptions;
};

VkCommandBuffer
BeginSingleTimeCommands(const Vulkan::IVulkanContextGetter& context);
void EndSingleTimeCommands(VkCommandBuffer commandBuffer,
                           const Vulkan::IVulkanContextGetter& context);

VertexLayoutInfo CreateVertexLayout(Renderer::EVertexLayout layoutType);

VulkanImage CreateImage(const Vulkan::IVulkanContextGetter& context,
                        Renderer::CMemoryAllocator& allocator, uint32_t width,
                        uint32_t height, VkFormat format, VkImageTiling tiling,
                        VkImageUsageFlags usage,
                        VkMemoryPropertyFlags properties);

void TransitionImageLayout(VkImage image, VkFormat format,
                           VkImageLayout oldLayout, VkImageLayout newLayout,
                           const Vulkan::CVulkanRendering& renderer);

void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                       uint32_t height,
                       const Vulkan::IVulkanContextGetter& context,
                       const Vulkan::CVulkanRendering& renderer);

void CreateImageView(VkDevice device, VkImage image, VkFormat format,
                     VkImageAspectFlags aspectFlags, VkImageView& imageView);

VkSampler CreateSampler(VkDevice device);

void CreateDescriptorPool(VkDevice device, VkDescriptorPool& descriptorPool,
                          std::size_t maxTextures);

void CreateTextureDescriptorSetLayout(
    VkDevice device, VkDescriptorSetLayout& descriptorSetLayout,
    std::size_t maxTextures);

VkDescriptorSet AllocateTextureDescriptorSet(VkDevice device,
                                             VkDescriptorPool pool,
                                             VkDescriptorSetLayout layout);
} // namespace Renderer