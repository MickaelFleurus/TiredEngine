#pragma once

#include <cstring>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "engine/utils/BufferMemoryBlocks.h"
#include "engine/vulkan/VulkanContext.h"

namespace Vulkan {

class CGPUBuffer {
public:
    explicit CGPUBuffer(const SContext& context, VkDeviceSize size,
                        VkBufferUsageFlags usage, bool hostVisible = false);
    ~CGPUBuffer();

    CGPUBuffer(const CGPUBuffer&) = delete;
    CGPUBuffer& operator=(const CGPUBuffer&) = delete;
    CGPUBuffer(CGPUBuffer&& other);
    CGPUBuffer& operator=(CGPUBuffer&& other);

    VmaAllocation GetAllocation() const;
    VkBuffer GetBuffer() const;
    Utils::CBufferMemoryBlocks& GetMemoryBlocks();

private:
    const SContext& mContext;

    VkBuffer mBuffer{VK_NULL_HANDLE};
    VmaAllocation mAllocation{VK_NULL_HANDLE};
    VkDeviceSize mSize{0};
    VkDeviceAddress mDeviceAddress{0};
    Utils::CBufferMemoryBlocks mMemoryBlocks;
};
} // namespace Vulkan
