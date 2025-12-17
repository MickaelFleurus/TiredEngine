#pragma once

#include <optional>
#include <vector>

#include <vulkan/vulkan.h>

#include "engine/vulkan/VulkanContext.h"

namespace Vulkan {
class CVulkanRendering {
public:
    CVulkanRendering(CVulkanContext& vulkanContext);
    ~CVulkanRendering();

    void Destroy();
    std::optional<uint32_t> AcquireNextImage();
    void SubmitSync(VkCommandBuffer commandBuffer, uint32_t imageIndex) const;
    void SubmitAsync(VkCommandBuffer commandBuffer, uint32_t imageIndex) const;
    void SubmitSyncSingleUse(VkCommandBuffer commandBuffer) const;
    void Present(uint32_t imageIndex);
    void WaitIdle() const;

    uint32_t GetFamilyIndex() const;

    void BeginRenderPass(uint32_t index, VkViewport viewport, VkRect2D scissor);
    void EndRenderPass(uint32_t index);

private:
    CVulkanContext& mVulkanContext;
    VkQueue mQueue = VK_NULL_HANDLE;
    VkSemaphore mImageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore mRenderFinishedSemaphore = VK_NULL_HANDLE;
    std::vector<VkFence> mFrameFences;
};
} // namespace Vulkan