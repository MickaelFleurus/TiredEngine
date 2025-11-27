#pragma once

#include <vector>

#include "engine/renderer/BufferHandle.h"

#include <memory>

namespace Vulkan {
class CVulkanContext;
}

namespace Renderer {

class CMemoryAllocator;
class CIndexesBufferHandleWrapper;
class CVertexBufferHandleWrapper;

class CBufferHandler {
public:
    CBufferHandler(const Vulkan::CVulkanContext& vulkanContext,
                   CMemoryAllocator& memoryAllocator);
    ~CBufferHandler();

    CBufferHandle* CreateBuffer(int dataSize, int size,
                                VkBufferUsageFlags usage);

    CVertexBufferHandleWrapper& GetVerticesBufferHandle();
    CIndexesBufferHandleWrapper& GetIndexesBufferHandle();
    CBufferHandle& GetInstanceBufferHandle();
    CBufferHandle& GetInstancesInfoBufferHandle();

    void BindBuffers(VkCommandBuffer commandBuffer);

    void FreeBuffer(CBufferHandle* bufferHandle);

private:
    const Vulkan::CVulkanContext& mVulkanContext;
    CMemoryAllocator& mMemoryAllocator;

    std::vector<std::unique_ptr<CBufferHandle>> mBuffers;
    std::unique_ptr<CVertexBufferHandleWrapper> mVerticesBufferHandle;
    std::unique_ptr<CIndexesBufferHandleWrapper> mIndexesBufferHandle;
};
} // namespace Renderer