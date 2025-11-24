#pragma once

#include "engine/vulkan/IVulkanContext.h"
#include <optional>
#include <vulkan/vulkan.h>

namespace Vulkan {
class CVulkanRendering {
public:
    CVulkanRendering(IVulkanContextGetter& vulkanContext);
    ~CVulkanRendering();

    void Destroy();
    std::optional<uint32_t> AcquireNextImage();
    void SubmitSync(VkCommandBuffer commandBuffer) const;
    void SubmitAsync(VkCommandBuffer commandBuffer) const;
    void Present(uint32_t imageIndex);
    void WaitIdle() const;

    VkQueue GetHandle() const;
    uint32_t GetFamilyIndex() const;

    void BeginRenderPass(uint32_t index, VkViewport viewport, VkRect2D scissor);
    void EndRenderPass(uint32_t index);

    VkCommandPool GetSingleTimeCommandPool() const;

private:
    IVulkanContextGetter& mVulkanContext;
    VkCommandPool mSingleTimeCommandPool = VK_NULL_HANDLE;
    VkQueue mQueue = VK_NULL_HANDLE;
    VkSemaphore mImageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore mRenderFinishedSemaphore = VK_NULL_HANDLE;
};
} // namespace Vulkan