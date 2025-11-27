#pragma once

#include "engine/utils/BufferMemoryBlocks.h"
#include <vulkan/vulkan.h>

namespace Vulkan {
class CVulkanContext;
}

namespace Renderer {

class CBufferHandler;
class CMemoryAllocator;
class CBufferHandle {
public:
    explicit CBufferHandle(const Vulkan::CVulkanContext& vulkanContext,
                           Renderer::CMemoryAllocator& memoryAllocator,
                           CBufferHandler& handler);

    CBufferHandle(const CBufferHandle&) = delete;
    CBufferHandle& operator=(const CBufferHandle&) = delete;
    CBufferHandle(CBufferHandle&&) noexcept;
    CBufferHandle& operator=(CBufferHandle&&) noexcept;

    ~CBufferHandle();

    bool Init(int dataSize, int bufferSize, VkBufferUsageFlags usage);

    std::optional<Utils::SBufferRange> Reserve(int size);
    bool Fill(const void* data, int size, int offset,
              const Utils::SBufferRange& range);
    void FreeRange(const Utils::SBufferRange& range);

    VkBuffer GetBuffer() const;

    int GetSize() const;

private:
    void Free();

    VkDevice mDevice;
    Renderer::CMemoryAllocator& mMemoryAllocator;
    CBufferHandler& mHandler;

    int mDataSize = 0;
    Utils::CBufferMemoryBlocks mMemoryBlocks;

    VkBuffer mBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mMemory = VK_NULL_HANDLE;
};
} // namespace Renderer