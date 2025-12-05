#include "engine/vulkan/BufferHandle.h"

#include "engine/renderer/MemoryAllocator.h"
#include "engine/utils/Logger.h"
#include "engine/vulkan/VulkanContext.h"

namespace Vulkan {

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
        Destroy();
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
    Destroy();
}

bool CBufferHandle::Init(int dataSize, int bufferSize,
                         VkBufferUsageFlags usage) {

    if (mBuffer != VK_NULL_HANDLE || mMemory != VK_NULL_HANDLE) {
        LOG_WARNING("Reinitializing an already initialized buffer!");
        Destroy();
    }
    mUsage = usage;
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
    VkDeviceSize atomSize = mMemoryAllocator.GetBufferMemoryAlignment();
    VkDeviceSize alignedSize = ((size + atomSize - 1) / atomSize) * atomSize;
    return mMemoryBlocks.Allocate(static_cast<int>(alignedSize));
}

bool CBufferHandle::Resize(Utils::SBufferRange& range, int size) {
    if (!mMemoryBlocks.Contains(range)) {
        LOG_ERROR("Trying to resize an unreserved range!");
        return false;
    }
    if (mMemoryBlocks.GetTotalSize() < range.offset + range.size + size) {
        LOG_ERROR("Trying to resize beyond buffer size!");
        return false;
    }

    mMemoryBlocks.Free(range);
    VkDeviceSize atomSize = mMemoryAllocator.GetBufferMemoryAlignment();
    VkDeviceSize alignedSize = ((size + atomSize - 1) / atomSize) * atomSize;
    auto newRangeOpt = mMemoryBlocks.Allocate(static_cast<int>(alignedSize));
    if (!newRangeOpt.has_value()) {
        LOG_FATAL("Failed to resize range in place!");
        return false;
    }
    range = newRangeOpt.value();
    return true;
}

bool CBufferHandle::Fill(const void* data, int size, int offset,
                         const Utils::SBufferRange& range) {

    if (!mMemoryBlocks.Contains(range)) {
        LOG_ERROR("Trying to fill data to an unreserved range!");
        return false;
    }

    // Calculate absolute offset in the buffer
    int absoluteOffset = range.offset + offset;

    // Check bounds: data must fit within the reserved range
    if (offset + size > range.size) {
        LOG_ERROR("Trying to fill more data than reserved!");
        return false;
    }

    VkDeviceSize atomSize = mMemoryAllocator.GetBufferMemoryAlignment();
    VkDeviceSize alignedOffset = (absoluteOffset / atomSize) * atomSize;
    VkDeviceSize end = absoluteOffset + size;
    VkDeviceSize alignedEnd = ((end + atomSize - 1) / atomSize) * atomSize;
    VkDeviceSize alignedSize = alignedEnd - alignedOffset;

    void* mappedData;
    VkResult result = vkMapMemory(mDevice, mMemory, alignedOffset, alignedSize,
                                  0, &mappedData);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to map memory!");
        return false;
    }

    memcpy(static_cast<uint8_t*>(mappedData) + (absoluteOffset - alignedOffset),
           data, size);

    VkMappedMemoryRange mappedRange{};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = mMemory;
    mappedRange.offset = alignedOffset;
    mappedRange.size = alignedSize;
    vkFlushMappedMemoryRanges(mDevice, 1, &mappedRange);

    vkUnmapMemory(mDevice, mMemory);
    return true;
}

void CBufferHandle::Destroy() {
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

void CBufferHandle::FreeRanges() {
    mMemoryBlocks.Reset();
}

VkBufferUsageFlags CBufferHandle::GetUsageFlags() const {
    return mUsage;
}

const Utils::CBufferMemoryBlocks& CBufferHandle::GetMemoryBlocks() const {
    return mMemoryBlocks;
}

} // namespace Vulkan