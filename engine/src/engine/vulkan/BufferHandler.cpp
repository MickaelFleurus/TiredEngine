#include "engine/vulkan/BufferHandler.h"

#include "engine/vulkan/GPUBuffer.h"

namespace {
Vulkan::CGPUBuffer CreateVertexBuffer(const Vulkan::SContext& context) {

    constexpr VkDeviceSize vertexBufferSize = 1024 * 1024 * 10; // 10 MB
    return Vulkan::CGPUBuffer(context, vertexBufferSize,
                              VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                  VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                  VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                              false);
}

Vulkan::CGPUBuffer CreateIndexBuffer(const Vulkan::SContext& context) {

    constexpr VkDeviceSize indexBufferSize = 1024 * 1024 * 10; // 10 MB
    return Vulkan::CGPUBuffer(context, indexBufferSize,
                              VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                  VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                              false);
}

} // namespace
namespace Vulkan {

CBufferHandler::CBufferHandler(const Vulkan::SContext& context)
    : mContext(context)
    , mStagingBuffer(context)
    , mQuadElementsBuffers(context)
    , mIndirectDrawBuffers(context)
    , mVertexBuffer(CreateVertexBuffer(context))
    , mIndexBuffer(CreateIndexBuffer(context))
    , mInstanceBuffer(CreateVertexBuffer(context))
    , mMaterialBuffer(CreateVertexBuffer(context))
    , mMeshInfoBuffer(CreateVertexBuffer(context))
    , mTransformBuffer(CreateVertexBuffer(context)) {
}

} // namespace Vulkan
