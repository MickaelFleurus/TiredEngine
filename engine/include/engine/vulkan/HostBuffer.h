#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "engine/utils/Logger.h"
#include "engine/vulkan/GPUBuffer.h"
#include "engine/vulkan/StagingBuffer.h"

namespace Vulkan {

struct SContext;

class CHostBuffer {
public:
    CHostBuffer(const SContext& context, VkDeviceSize size,
                VkBufferUsageFlags usage);

    template <typename T>
    bool ReplaceData(const T& data) {
        return ReplaceData(std::vector<T>{data});
    }

    template <typename T>
    bool ReplaceData(const std::vector<T>& data) {
        mBuffer.GetMemoryBlocks().Reset();
        return AppendData(data);
    }

    template <typename T>
    bool AppendData(const T& data) {
        return AppendData(std::vector<T>{data});
    }

    template <typename T>
    bool AppendData(const std::vector<T>& data) {
        const auto alignment = mContext.physicalDeviceProperties.limits
                                   .minStorageBufferOffsetAlignment;
        const auto alignedItemSize =
            (sizeof(T) + alignment - 1) / alignment * alignment;
        const auto totalSize = alignedItemSize * data.size();

        const auto optRange = mBuffer.GetMemoryBlocks().Allocate(totalSize);
        if (!optRange.has_value()) {
            LOG_FATAL("Not enough space to append data in data buffer!");
            return false;
        }
        return WriteData(reinterpret_cast<const uint8_t*>(data.data()),
                         sizeof(T), alignedItemSize, data.size(),
                         optRange->offset);
    }

    template <typename T>
    bool UpdateData(const T& data, const Utils::SBufferRange& range) {
        return UpdateData(std::vector<T>{data}, range);
    }

    // Overwrites an already-allocated range (e.g. one previously returned
    // via AppendData) with new data. Same host-visible-vs-device-local
    // rules as AppendData. Caller is responsible for `range` fitting `data`.
    template <typename T>
    bool UpdateData(const std::vector<T>& data,
                    const Utils::SBufferRange& range) {
        if (mBuffer.GetMemoryBlocks().Contains(range)) {
            LOG_FATAL("UpdateData: range does not belong to this buffer!");
            return false;
        }
        const auto alignment = mContext.physicalDeviceProperties.limits
                                   .minStorageBufferOffsetAlignment;
        const auto alignedItemSize =
            (sizeof(T) + alignment - 1) / alignment * alignment;
        const auto totalSize = alignedItemSize * data.size();
        if (totalSize > range.size) {
            LOG_FATAL("UpdateData: data does not fit in supplied range!");
            return false;
        }
        return WriteData(reinterpret_cast<const uint8_t*>(data.data()),
                         sizeof(T), alignedItemSize, data.size(), range.offset);
    }

    VkDeviceAddress GetDeviceAddress() const;

private:
    bool WriteData(const uint8_t* items, size_t itemSize,
                   size_t alignedItemSize, size_t itemCount,
                   VkDeviceSize dstOffset);

    const SContext& mContext;

    CGPUBuffer mBuffer;
    void* mMappedPtr = nullptr;
    VkDeviceAddress mDeviceAddress = 0;
};
} // namespace Vulkan
