#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan {
class IVulkanContextGetter;
}

namespace Renderer {

class CBufferHandler;
class CMemoryAllocator;
class CBufferHandle {
public:
    explicit CBufferHandle(const Vulkan::IVulkanContextGetter& vulkanContext,
                           Renderer::CMemoryAllocator& memoryAllocator,
                           CBufferHandler& handler, int id);
    ~CBufferHandle();

    bool Init(int dataSize, int bufferSize, VkBufferUsageFlags usage);
    bool Fill(const void* data, int size, int offset);
    bool Append(const void* data, int size);

    VkBuffer GetBuffer() const {
        return mBuffer;
    }

private:
    void Free();

    VkDevice mDevice;
    Renderer::CMemoryAllocator& mMemoryAllocator;
    CBufferHandler& mHandler;
    const int mId;

    int mDataSize = 0;
    int mBufferSize = 0;
    int mUsedSize = 0;

    VkBuffer mBuffer;
    VkDeviceMemory mMemory;
};
} // namespace Renderer