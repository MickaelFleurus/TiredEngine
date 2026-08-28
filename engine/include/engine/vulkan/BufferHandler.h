#pragma once

#include "engine/vulkan/GPUBuffer.h"
#include "engine/vulkan/IndirectDrawBuffers.h"
#include "engine/vulkan/QuadElementsBuffers.h"
#include "engine/vulkan/StagingBuffer.h"

namespace Vulkan {

struct SContext;

class CBufferHandler {
public:
    CBufferHandler(const SContext& context);

    CStagingBuffer& GetStagingBuffer() {
        return mStagingBuffer;
    }

    CQuadElementsBuffers& GetQuadElementsBuffers() {
        return mQuadElementsBuffers;
    }

    CIndirectDrawBuffers& GetIndirectDrawBuffers() {
        return mIndirectDrawBuffers;
    }

private:
    const SContext& mContext;

    CStagingBuffer mStagingBuffer;
    CQuadElementsBuffers mQuadElementsBuffers;
    CIndirectDrawBuffers mIndirectDrawBuffers;
    CGPUBuffer mVertexBuffer;
    CGPUBuffer mIndexBuffer;
    CGPUBuffer mInstanceBuffer;
    CGPUBuffer mMaterialBuffer;
    CGPUBuffer mMeshInfoBuffer;
    CGPUBuffer mTransformBuffer;
};
} // namespace Vulkan
