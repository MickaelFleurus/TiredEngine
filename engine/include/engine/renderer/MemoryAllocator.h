#pragma once
#include <vulkan/vulkan.h>

namespace Vulkan {
struct SContext;
}

namespace Renderer {
class CMemoryAllocator {
public:
    CMemoryAllocator(Vulkan::SContext& context);

    VkDeviceMemory AllocateMemory(VkImage image,
                                  VkMemoryPropertyFlags properties);

    VkDeviceMemory AllocateMemory(VkBuffer buffer,
                                  VkMemoryPropertyFlags properties);

    VkDeviceSize GetBufferMemoryAlignment() const;
    VkDeviceSize AlignSize(VkDeviceSize size) const;

private:
    Vulkan::SContext& mContext;
};
} // namespace Renderer
