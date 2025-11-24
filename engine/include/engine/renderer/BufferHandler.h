#pragma once

#include <vector>

#include "engine/renderer/BufferHandle.h"

namespace Vulkan {
class IVulkanContextGetter;
}

namespace Renderer {
class CMemoryAllocator;
class CBufferHandler {
public:
    CBufferHandler(const Vulkan::IVulkanContextGetter& vulkanContext,
                   CMemoryAllocator& memoryAllocator);

    CBufferHandle* CreateBuffer(int size, int dataSize,
                                VkBufferUsageFlags usage);

private:
    const Vulkan::IVulkanContextGetter& mVulkanContext;
    CMemoryAllocator& mMemoryAllocator;

    std::vector<CBufferHandle> mBuffers;
};
} // namespace Renderer