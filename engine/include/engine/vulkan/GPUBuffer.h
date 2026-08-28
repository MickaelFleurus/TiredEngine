#pragma once

#include <cstring>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "engine/utils/BufferMemoryBlocks.h"
#include "engine/vulkan/VulkanContext.h"

namespace Vulkan {

struct SBufferOperation {
    Utils::SBufferRange range; // The range of the buffer to update
    size_t stagingOffset;      // Where the data lives in our staging arena
};

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

    // template <typename T>
    // bool AppendData(T& data) {
    //     return AppendData(std::vector<T>{data});
    // }

    // template <typename T>
    // bool AppendData(std::vector<T>& data) {
    //     const auto alignment = mContext.physicalDeviceProperties.limits
    //                                .minStorageBufferOffsetAlignment;
    //     const auto alignedDataSize = VmaAlignUp(sizeof(T), alignment);
    //     const auto totalDataSize = alignedDataSize * data.size();

    //     const auto optRange = mMemoryBlocks.Allocate(totalDataSize);
    //     if (!optRange.has_value()) {
    //         LOG_FATAL("Not enough space to append data in data buffer!");
    //         return false;
    //     }
    //     const auto stagingOffset = mStaging.size();
    //     mStaging.resize(stagingOffset + totalDataSize);
    //     for (const auto& item : data) {
    //         std::memcpy(mStaging.data() + stagingOffset, &item, sizeof(T));
    //         mOperations.push_back(SBufferOperation{
    //             .range = *optRange, .stagingOffset = stagingOffset});
    //     }
    //     return true;
    // }

    // template <typename T>
    // bool UpdateData(T& data, const Utils::SBufferRange& range) {
    //     return UpdateData(std::vector<T>{data}, range);
    // }

    // template <typename T>
    // bool UpdateData(std::vector<T>& data, const Utils::SBufferRange& range) {
    // }

private:
    const SContext& mContext;

    VkBuffer mBuffer{VK_NULL_HANDLE};
    VmaAllocation mAllocation{VK_NULL_HANDLE};
    VkDeviceSize mSize{0};
    VkDeviceAddress mDeviceAddress{0};
    Utils::CBufferMemoryBlocks mMemoryBlocks;
};
} // namespace Vulkan
