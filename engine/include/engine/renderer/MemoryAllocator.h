#pragma once
#include <vulkan/vulkan.h>

namespace Vulkan {
class CVulkanContext;
}

namespace Renderer {
class CMemoryAllocator {
public:
    CMemoryAllocator(Vulkan::CVulkanContext& vulkanContext);

    VkDeviceMemory AllocateMemory(VkImage image,
                                  VkMemoryPropertyFlags properties);

    VkDeviceMemory AllocateMemory(VkBuffer buffer,
                                  VkMemoryPropertyFlags properties);

    VkDeviceSize GetBufferMemoryAlignment() const;

private:
    Vulkan::CVulkanContext& mVulkanContext;
};
} // namespace Renderer