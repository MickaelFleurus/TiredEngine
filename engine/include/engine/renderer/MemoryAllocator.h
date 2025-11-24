#pragma once
#include <vulkan/vulkan.h>

namespace Vulkan {
class IVulkanContextGetter;
}

namespace Renderer {
class CMemoryAllocator {
public:
    CMemoryAllocator(Vulkan::IVulkanContextGetter& vulkanContext);

    VkDeviceMemory AllocateMemory(VkImage image,
                                  VkMemoryPropertyFlags properties);

    VkDeviceMemory AllocateMemory(VkBuffer buffer,
                                  VkMemoryPropertyFlags properties);

private:
    Vulkan::IVulkanContextGetter& mVulkanContext;
};
} // namespace Renderer