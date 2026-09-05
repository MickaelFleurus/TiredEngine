#pragma once

#include "engine/vulkan/DeviceBuffer.h"
#include "engine/vulkan/GPUBuffer.h"
#include "engine/vulkan/HostBuffer.h"
#include "engine/vulkan/IndirectDrawBuffers.h"
#include "engine/vulkan/StagingBuffer.h"

namespace Vulkan {

struct SContext;

class CBufferHandler {
public:
    CBufferHandler(const SContext& context);

    CStagingBuffer& GetStagingBuffer() {
        return mStagingBuffer;
    }

    CIndirectDrawBuffers& GetIndirectDrawBuffers() {
        return mIndirectDrawBuffers;
    }

    CHostBuffer& GetUiInstancesBuffer() {
        return mUiInstancesBuffer;
    }

private:
    const SContext& mContext;

    CStagingBuffer mStagingBuffer;
    CHostBuffer mUiInstancesBuffer;

    CIndirectDrawBuffers mIndirectDrawBuffers;
    CGPUBuffer mVertexBuffer;
    CGPUBuffer mIndexBuffer;
    CGPUBuffer mInstanceBuffer;
    CGPUBuffer mMaterialBuffer;
    CGPUBuffer mMeshInfoBuffer;
    CGPUBuffer mTransformBuffer;
};
} // namespace Vulkan
