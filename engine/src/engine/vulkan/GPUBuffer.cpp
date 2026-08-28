#include "engine/vulkan/GPUBuffer.h"

#include "engine/utils/Logger.h"
#include "engine/vulkan/VulkanContext.h"

namespace Vulkan {
CGPUBuffer::CGPUBuffer(const SContext& context, VkDeviceSize size,
                       VkBufferUsageFlags usage, bool hostVisible)
    : mContext(context) {
    VkBufferCreateInfo bufferInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = hostVisible ? VMA_MEMORY_USAGE_AUTO_PREFER_HOST
                                  : VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    if (hostVisible) {
        allocInfo.flags =
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
            VMA_ALLOCATION_CREATE_MAPPED_BIT;
    }

    VmaAllocationInfo allocResult{};
    if (vmaCreateBuffer(mContext.vmaAllocator, &bufferInfo, &allocInfo,
                        &mBuffer, &mAllocation, &allocResult)) {
        LOG_FATAL("Failed to create GPU buffer!");
        return;
    }

    if ((usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) != 0) {
        VkBufferDeviceAddressInfo addrInfo{
            VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO};
        addrInfo.buffer = mBuffer;
        mDeviceAddress = vkGetBufferDeviceAddress(mContext.device, &addrInfo);
    }
}

CGPUBuffer::CGPUBuffer(CGPUBuffer&& other)
    : mContext(other.mContext)
    , mBuffer(other.mBuffer)
    , mAllocation(other.mAllocation)
    , mSize(other.mSize)
    , mDeviceAddress(other.mDeviceAddress)
    , mMemoryBlocks(std::move(other.mMemoryBlocks)) {
    other.mBuffer = VK_NULL_HANDLE;
    other.mAllocation = VK_NULL_HANDLE;
    other.mDeviceAddress = 0;
    other.mSize = 0;
}

CGPUBuffer& CGPUBuffer::operator=(CGPUBuffer&& other) {
    if (this == &other) {
        return *this;
    }

    if (mBuffer != VK_NULL_HANDLE) {
        vmaDestroyBuffer(mContext.vmaAllocator, mBuffer, mAllocation);
    }

    mBuffer = other.mBuffer;
    mAllocation = other.mAllocation;
    mSize = other.mSize;
    mDeviceAddress = other.mDeviceAddress;
    mMemoryBlocks = std::move(other.mMemoryBlocks);

    other.mBuffer = VK_NULL_HANDLE;
    other.mAllocation = VK_NULL_HANDLE;
    other.mDeviceAddress = 0;
    other.mSize = 0;
    return *this;
}

CGPUBuffer::~CGPUBuffer() {
    if (mBuffer != VK_NULL_HANDLE) {
        vmaDestroyBuffer(mContext.vmaAllocator, mBuffer, mAllocation);
        mBuffer = VK_NULL_HANDLE;
        mAllocation = VK_NULL_HANDLE;
    }
}

VmaAllocation CGPUBuffer::GetAllocation() const {
    return mAllocation;
}

VkBuffer CGPUBuffer::GetBuffer() const {
    return mBuffer;
}
} // namespace Vulkan
