#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "engine/vulkan/GPUBuffer.h"

namespace Vulkan {
struct SContext;

class CIndirectDrawBuffers {
public:
    struct PipelineDrawGroup {
        VkPipeline pipeline;
        Vulkan::CGPUBuffer indirectBuffer;
        Vulkan::CGPUBuffer countBuffer;
    };

    CIndirectDrawBuffers(const Vulkan::SContext& context);

    void CreatePipelineDrawGroup(VkPipeline pipeline);
    void ResetDrawGroupCounts(VkCommandBuffer cmd);

    const std::vector<PipelineDrawGroup>& GetDrawGroups() const {
        return mPipelineDrawGroups;
    }

private:
    const Vulkan::SContext& mContext;
    std::vector<PipelineDrawGroup> mPipelineDrawGroups;
};
} // namespace Vulkan
