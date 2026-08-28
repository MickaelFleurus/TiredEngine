#include "engine/vulkan/HostBuffer.h"

namespace Vulkan {
CHostBuffer::CHostBuffer(const SContext& context, VkDeviceSize size,
                         VkBufferUsageFlags usage)
    : mContext(context), mBuffer(context, size, usage, true) {
    VmaAllocationInfo info;
    vmaGetAllocationInfo(mContext.vmaAllocator, mBuffer.GetAllocation(), &info);
    mMappedPtr = info.pMappedData;
}

bool CHostBuffer::WriteData(const uint8_t* items, size_t itemSize,
                            size_t alignedItemSize, size_t itemCount,
                            VkDeviceSize dstOffset) {

    auto* dst = static_cast<uint8_t*>(mMappedPtr) + dstOffset;
    for (size_t i = 0; i < itemCount; ++i) {
        std::memcpy(dst + i * alignedItemSize, items + i * itemSize, itemSize);
    }
    return true;
}
} // namespace Vulkan
