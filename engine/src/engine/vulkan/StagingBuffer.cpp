#include "engine/vulkan/StagingBuffer.h"

namespace {
constexpr VkDeviceSize kStagingBufferSize = 64 * 1024 * 1024; // 1 MB
}

namespace Vulkan {
CStagingBuffer::CStagingBuffer(const SContext& context)
    : mContext(context)
    , mStagingBuffer(context, kStagingBufferSize,
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                         VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                         VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                     true) {
    VmaAllocationInfo info;
    vmaGetAllocationInfo(mContext.vmaAllocator, mStagingBuffer.GetAllocation(),
                         &info);
    mMappedPtr = info.pMappedData;

    VkCommandPoolCreateInfo poolInfo{
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    poolInfo.queueFamilyIndex = mContext.transferQueueFamilyIndex;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    vkCreateCommandPool(mContext.device, &poolInfo, nullptr, &mCmdPool);

    VkCommandBufferAllocateInfo cbInfo{
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    cbInfo.commandPool = mCmdPool;
    cbInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cbInfo.commandBufferCount = 1;
    vkAllocateCommandBuffers(mContext.device, &cbInfo, &mCmdBuffer);

    VkFenceCreateInfo fenceInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    vkCreateFence(mContext.device, &fenceInfo, nullptr, &mFence);
}

void CStagingBuffer::UploadToBuffer(const void* srcData, VkDeviceSize size,
                                    VkBuffer dstBuffer,
                                    VkDeviceSize dstOffset) {
    if (mWriteCursor + size > kStagingBufferSize) {
        Flush(); // ring is full, push what we have and start over
    }

    memcpy(static_cast<uint8_t*>(mMappedPtr) + mWriteCursor, srcData, size);

    VkBufferCopy region{};
    region.srcOffset = mWriteCursor;
    region.dstOffset = dstOffset;
    region.size = size;
    vkCmdCopyBuffer(mCmdBuffer, mStagingBuffer.GetBuffer(), dstBuffer, 1,
                    &region);

    mWriteCursor += size;
}

void CStagingBuffer::UploadToImage(const void* srcData, VkDeviceSize size,
                                   VkImage dstImage, uint32_t width,
                                   uint32_t height) {
    if (mWriteCursor + size > kStagingBufferSize) {
        Flush();
    }

    memcpy(static_cast<uint8_t*>(mMappedPtr) + mWriteCursor, srcData, size);

    VkBufferImageCopy region{};
    region.bufferOffset = mWriteCursor;
    region.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    region.imageExtent = {width, height, 1};
    vkCmdCopyBufferToImage(mCmdBuffer, mStagingBuffer.GetBuffer(), dstImage,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    mWriteCursor += size;
}

void CStagingBuffer::Flush() {
    vkEndCommandBuffer(mCmdBuffer);

    VkSubmitInfo submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &mCmdBuffer;
    vkQueueSubmit(mContext.transferQueue, 1, &submit, mFence);

    vkWaitForFences(mContext.device, 1, &mFence, VK_TRUE, UINT64_MAX);
    vkResetFences(mContext.device, 1, &mFence);

    vkResetCommandBuffer(mCmdBuffer, 0);
    VkCommandBufferBeginInfo beginInfo{
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(mCmdBuffer, &beginInfo);

    mWriteCursor =
        0; // ring buffer resets since we waited for GPU to finish reading it
}
} // namespace Vulkan
