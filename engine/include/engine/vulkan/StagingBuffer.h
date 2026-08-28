#pragma once

#include <vulkan/vulkan.h>

#include "engine/vulkan/GPUBuffer.h"

namespace Vulkan {

struct SContext;

class CStagingBuffer {
public:
    CStagingBuffer(const SContext& context);
    ~CStagingBuffer();

    void UploadToBuffer(const void* srcData, VkDeviceSize size,
                        VkBuffer dstBuffer, VkDeviceSize dstOffset);
    void UploadToImage(const void* srcData, VkDeviceSize size, VkImage dstImage,
                       uint32_t width, uint32_t height);
    void Flush();

    VkCommandBuffer GetCmdBuffer() const {
        return mCmdBuffer;
    }

private:
    const SContext& mContext;

    VkCommandPool mCmdPool{VK_NULL_HANDLE};
    VkCommandBuffer mCmdBuffer{VK_NULL_HANDLE};
    VkFence mFence{VK_NULL_HANDLE};

    CGPUBuffer mStagingBuffer;
    void* mMappedPtr = nullptr;
    VkDeviceSize mWriteCursor = 0;
};
} // namespace Vulkan
