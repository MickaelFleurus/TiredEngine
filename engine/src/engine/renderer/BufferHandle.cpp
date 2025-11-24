#include "engine/renderer/BufferHandle.h"
#include "engine/vulkan/IVulkanContext.h"

#include "engine/renderer/MemoryAllocator.h"

#include "engine/utils/Logger.h"

namespace Renderer {

CBufferHandle::CBufferHandle(const Vulkan::IVulkanContextGetter& vulkanContext,
                             Renderer::CMemoryAllocator& memoryAllocator,
                             CBufferHandler& handler, int id)
    : mDevice(vulkanContext.GetDevice())
    , mMemoryAllocator(memoryAllocator)
    , mHandler(handler)
    , mId(id) {
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
    mBufferSize = bufferSize;

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

bool CBufferHandle::Fill(const void* data, int size, int offset) {
    if (size + offset > mBufferSize) {
        LOG_ERROR("Buffer overflow on fill!");
        return false;
    }
    void* mappedData;
    vkMapMemory(mDevice, mMemory, 0, size, 0, &mappedData);
    memcpy(mappedData, data, size);
    VkMappedMemoryRange range{};
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = mMemory;
    range.offset = offset;
    range.size = size;
    vkFlushMappedMemoryRanges(mDevice, 1, &range);
    vkUnmapMemory(mDevice, mMemory);
    mUsedSize = offset + size; // FIXME: Probably incorrect but does it matter?
    return true;
}

bool CBufferHandle::Append(const void* data, int size) {
    if (mUsedSize + size > mBufferSize) {
        LOG_ERROR("Buffer overflow on append!");
        return false;
    }
    Fill(data, size, mUsedSize);
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
}

} // namespace Renderer