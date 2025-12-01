#pragma once

#include <vector>

#include "engine/vulkan/BufferHandle.h"

#include <memory>

namespace Renderer {
class CMemoryAllocator;
}

namespace Vulkan {

class CVulkanContext;
class CIndexesBufferHandleWrapper;
class CVertexBufferHandleWrapper;

class CBufferHandler {
public:
    CBufferHandler(const Vulkan::CVulkanContext& vulkanContext,
                   Renderer::CMemoryAllocator& memoryAllocator);
    ~CBufferHandler();

    CBufferHandle* CreateBuffer(int dataSize, int size,
                                VkBufferUsageFlags usage);

    CVertexBufferHandleWrapper& GetVerticesBufferHandle();
    CIndexesBufferHandleWrapper& GetIndexesBufferHandle();
    CBufferHandle& GetInstanceBufferHandle();
    CBufferHandle& GetInstancesInfoBufferHandle();
    CBufferHandle& GetTextInstanceBufferHandle();
    CBufferHandle& GetTextInstancesInfoBufferHandle();

    void BindBuffers(VkCommandBuffer commandBuffer);

    void FreeBuffer(CBufferHandle* bufferHandle);

private:
    const Vulkan::CVulkanContext& mVulkanContext;
    Renderer::CMemoryAllocator& mMemoryAllocator;

    std::vector<std::unique_ptr<CBufferHandle>> mBuffers;
    std::unique_ptr<CVertexBufferHandleWrapper> mVerticesBufferHandle;
    std::unique_ptr<CIndexesBufferHandleWrapper> mIndexesBufferHandle;
};
} // namespace Vulkan