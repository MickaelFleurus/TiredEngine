#include "engine/renderer/BufferHandle.h"
#include "engine/vulkan/VulkanContext.h"

#include "engine/renderer/MemoryAllocator.h"

#include "engine/utils/Logger.h"

namespace Renderer {

CBufferHandle::CBufferHandle(const Vulkan::CVulkanContext& vulkanContext,
                             Renderer::CMemoryAllocator& memoryAllocator,
                             CBufferHandler& handler)
    : mDevice(vulkanContext.GetDevice())
    , mMemoryAllocator(memoryAllocator)
    , mHandler(handler) {
}

CBufferHandle::CBufferHandle(CBufferHandle&& other) noexcept
    : mDevice(other.mDevice)
    , mMemoryAllocator(other.mMemoryAllocator)
    , mHandler(other.mHandler)
    , mDataSize(other.mDataSize)
    , mMemoryBlocks(std::move(other.mMemoryBlocks))
    , mBuffer(other.mBuffer)
    , mMemory(other.mMemory) {
    other.mBuffer = VK_NULL_HANDLE;
    other.mMemory = VK_NULL_HANDLE;
}

CBufferHandle& CBufferHandle::operator=(CBufferHandle&& other) noexcept {
    if (this != &other) {
        Free();
        mDataSize = other.mDataSize;
        mMemoryBlocks = std::move(other.mMemoryBlocks);
        mBuffer = other.mBuffer;
        mMemory = other.mMemory;

        other.mBuffer = VK_NULL_HANDLE;
        other.mMemory = VK_NULL_HANDLE;
    }
    return *this;
}

CBufferHandle::~CBufferHandle() {
    Free();
}

bool CBufferHandle::Init(int dataSize, int bufferSize,
                         VkBufferUsageFlags usage) {

    if (mBuffer != VK_NULL_HANDLE || mMemory != VK_NULL_HANDLE) {
        LOG_WARNING("Reinitializing an already initialized buffer!");
        Free();
    }
    mDataSize = dataSize;
    mMemoryBlocks.Init(bufferSize);

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.usage = usage;
    bufferInfo.size = bufferSize;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(mDevice, &bufferInfo, nullptr, &mBuffer) != VK_SUCCESS) {
        LOG_ERROR("Failed to create buffer!");
        return false;
    }

    mMemory = mMemoryAllocator.AllocateMemory(
        mBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vkBindBufferMemory(mDevice, mBuffer, mMemory, 0);
    return true;
}
std::optional<Utils::SBufferRange> CBufferHandle::Reserve(int size) {
    return mMemoryBlocks.Allocate(size);
}

bool CBufferHandle::Fill(const void* data, int size, int offset,
                         const Utils::SBufferRange& range) {

    if (offset + size > range.size) {
        LOG_ERROR("Trying to fill more data than reserved!");
        return false;
    }

    if (!mMemoryBlocks.Contains(range)) {
        LOG_ERROR("Trying to fill data to an unreserved range!");
        return false;
    }
    VkDeviceSize atomSize = mMemoryAllocator.GetBufferMemoryAlignment();
    VkDeviceSize alignedOffset = (offset / atomSize) * atomSize;
    VkDeviceSize end = offset + size;
    VkDeviceSize alignedEnd = ((end + atomSize - 1) / atomSize) * atomSize;
    VkDeviceSize alignedSize = alignedEnd - alignedOffset;

    void* mappedData;
    vkMapMemory(mDevice, mMemory, alignedOffset, alignedSize, 0, &mappedData);
    memcpy(mappedData, data, size);
    VkMappedMemoryRange mappedRange{};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = mMemory;
    mappedRange.offset = alignedOffset;
    mappedRange.size = alignedSize;
    vkFlushMappedMemoryRanges(mDevice, 1, &mappedRange);
    vkUnmapMemory(mDevice, mMemory);
    return true;
}

void CBufferHandle::Free() {
    if (mBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(mDevice, mBuffer, nullptr);
        mBuffer = VK_NULL_HANDLE;
    }
    if (mMemory != VK_NULL_HANDLE) {
        vkFreeMemory(mDevice, mMemory, nullptr);
        mMemory = VK_NULL_HANDLE;
    }
    mDataSize = 0;
    mMemoryBlocks = {};
}

VkBuffer CBufferHandle::GetBuffer() const {
    return mBuffer;
}

int CBufferHandle::GetSize() const {
    return mMemoryBlocks.GetTotalSize();
}

void CBufferHandle::FreeRange(const Utils::SBufferRange& range) {
    if (!mMemoryBlocks.Contains(range)) {
        LOG_WARNING("Trying to free a non-existing range!");
        return;
    }
    mMemoryBlocks.Free(range);
}

} // namespace Renderer