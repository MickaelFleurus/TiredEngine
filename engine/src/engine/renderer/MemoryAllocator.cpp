#include "engine/renderer/MemoryAllocator.h"
#include "engine/utils/Logger.h"

#include "engine/vulkan/IVulkanContext.h"

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

namespace Renderer {

CMemoryAllocator::CMemoryAllocator(Vulkan::IVulkanContextGetter& vulkanContext)
    : mVulkanContext(vulkanContext) {
}

VkDeviceMemory
CMemoryAllocator::AllocateMemory(VkImage image,
                                 VkMemoryPropertyFlags properties) {
    VkDevice device = mVulkanContext.GetDevice();
    VkPhysicalDeviceMemoryProperties memProperties =
        mVulkanContext.GetPhysicalDeviceMemoryProperties();

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex =
        FindMemoryType(memRequirements.memoryTypeBits, memProperties,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    VkDeviceMemory memory;
    if (vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
        LOG_ERROR("Failed to allocate image memory!");
        return VK_NULL_HANDLE;
    }
    return memory;
}

VkDeviceMemory
CMemoryAllocator::AllocateMemory(VkBuffer buffer,
                                 VkMemoryPropertyFlags properties) {
    VkDevice device = mVulkanContext.GetDevice();
    VkPhysicalDeviceMemoryProperties memProperties =
        mVulkanContext.GetPhysicalDeviceMemoryProperties();

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex =
        FindMemoryType(memRequirements.memoryTypeBits, memProperties,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    VkDeviceMemory memory;
    if (vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
        LOG_ERROR("Failed to allocate buffer memory!");
        return VK_NULL_HANDLE;
    }
    return memory;
}
} // namespace Renderer