#include "engine/vulkan/IndirectDrawBuffers.h"

#include "engine/core/DataTypes.h"
#include "engine/vulkan/Constants.h"
#include "engine/vulkan/GPUBuffer.h"
#include "engine/vulkan/VulkanContext.h"

namespace {

Vulkan::CGPUBuffer createIndirectBuffer(const Vulkan::SContext& context) {
    constexpr auto size =
        sizeof(Core::SIndirectDrawCommand) * Vulkan::kMaxDrawCount;
    constexpr auto usage = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT |
                           VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                           VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                           VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

    Vulkan::CGPUBuffer out{context, size, usage};
    return out;
}

Vulkan::CGPUBuffer createCountBuffer(const Vulkan::SContext& context) {
    VkBufferCreateInfo bufferInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferInfo.size = sizeof(uint32_t);
    bufferInfo.usage = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT |
                       VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                       VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                       VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

    Vulkan::CGPUBuffer out{context, bufferInfo.size, bufferInfo.usage};
    return out;
}
} // namespace

namespace Vulkan {
CIndirectDrawBuffers::CIndirectDrawBuffers(const Vulkan::SContext& context)
    : mContext(context) {
}

void CIndirectDrawBuffers::CreatePipelineDrawGroup(VkPipeline pipeline) {
    PipelineDrawGroup group{.pipeline = pipeline,
                            .indirectBuffer = createIndirectBuffer(mContext),
                            .countBuffer = createCountBuffer(mContext)};
    mPipelineDrawGroups.push_back(std::move(group));
}

void CIndirectDrawBuffers::ResetDrawGroupCounts(VkCommandBuffer cmd) {
    for (auto& group : mPipelineDrawGroups) {
        vkCmdFillBuffer(cmd, group.countBuffer.GetBuffer(), 0, sizeof(uint32_t),
                        0);
    }
    // TODO : Figure out how this works
    //  barrier: TRANSFER_WRITE (the fill) -> SHADER_WRITE (the compute pass's
    //  atomic increment)
    VkMemoryBarrier2 barrier{VK_STRUCTURE_TYPE_MEMORY_BARRIER_2};
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;

    VkDependencyInfo depInfo{VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
    depInfo.memoryBarrierCount = 1;
    depInfo.pMemoryBarriers = &barrier;
    vkCmdPipelineBarrier2(cmd, &depInfo);
}
} // namespace Vulkan
