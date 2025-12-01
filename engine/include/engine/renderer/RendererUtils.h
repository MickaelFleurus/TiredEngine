#pragma once

#include "engine/material/AbstractMaterial.h"
#include "engine/renderer/PipelineTypes.h"

#include <concepts>
#include <cstdint>
#include <glm/mat4x4.hpp>
#include <ranges>
#include <span>
#include <vulkan/vulkan.h>

struct SDL_GPUBuffer;

namespace Vulkan {
class CVulkanContext;
class CVulkanRendering;
} // namespace Vulkan

namespace Renderer {
class CMemoryAllocator;

struct SRenderable {
    glm::mat4 transform;
    Material::CAbstractMaterial* material;
    float depth;
    std::size_t meshHash;
    glm::vec4 color;
    int textureIndex;

    bool operator<(const SRenderable& other) const {
        return material->GetPipeline() < other.material->GetPipeline();
    }

    bool operator==(const SRenderable& other) const {
        return transform == other.transform && material == other.material &&
               depth == other.depth && meshHash == other.meshHash &&
               color == other.color && textureIndex == other.textureIndex;
        ;
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

VkCommandBuffer BeginSingleTimeCommands(const Vulkan::CVulkanContext& context);
void EndSingleTimeCommands(VkCommandBuffer commandBuffer,
                           const Vulkan::CVulkanContext& context);

VertexLayoutInfo CreateVertexLayout(Renderer::EVertexLayout layoutType);

VulkanImage CreateImage(const Vulkan::CVulkanContext& context,
                        Renderer::CMemoryAllocator& allocator, uint32_t width,
                        uint32_t height, VkFormat format, VkImageTiling tiling,
                        VkImageUsageFlags usage,
                        VkMemoryPropertyFlags properties);

void TransitionImageLayout(VkImage image, VkFormat format,
                           VkImageLayout oldLayout, VkImageLayout newLayout,
                           const Vulkan::CVulkanContext& context,
                           Vulkan::CVulkanRendering& renderer);

void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                       uint32_t height, const Vulkan::CVulkanContext& context,
                       Vulkan::CVulkanRendering& renderer);

void CreateImageView(VkDevice device, VkImage image, VkFormat format,
                     VkImageAspectFlags aspectFlags, VkImageView& imageView);

VkSampler CreateSampler(VkDevice device);

} // namespace Renderer