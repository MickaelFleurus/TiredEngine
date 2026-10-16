#include "engine/vulkan/DeviceBuffer.h"

#include <cstring>

namespace Vulkan {
CDeviceBuffer::CDeviceBuffer(const SContext& context, VkDeviceSize size,
                             VkBufferUsageFlags usage,
                             CStagingBuffer& stagingBuffer)
    : mContext(context)
    , mBuffer(context, size, usage)
    , mStagingBuffer(stagingBuffer) {
}

bool CDeviceBuffer::WriteData(const uint8_t* items, size_t itemSize,
                              size_t alignedItemSize, size_t itemCount,
                              VkDeviceSize dstOffset) {
    if (itemCount == 0) {
        return true;
    }

    SPendingWrite pendingWrite;
    pendingWrite.data.resize(alignedItemSize * itemCount);
    for (size_t i = 0; i < itemCount; ++i) {
        std::memcpy(pendingWrite.data.data() + i * alignedItemSize,
                    items + i * itemSize, itemSize);
    }
    pendingWrite.dstOffset = dstOffset;
    mPendingWrites.push_back(std::move(pendingWrite));
    return true;
}

void CDeviceBuffer::Flush() {
    for (const auto& pendingWrite : mPendingWrites) {
        mStagingBuffer.UploadToBuffer(
            pendingWrite.data.data(), pendingWrite.data.size(),
            mBuffer.GetBuffer(), pendingWrite.dstOffset);
    }
    mPendingWrites.clear();
    mStagingBuffer.Flush();
}
} // namespace Vulkan
