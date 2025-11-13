#pragma once

#include <optional>
#include <vulkan/vulkan.h>

namespace Renderer {
class CVulkanQueue {
public:
    CVulkanQueue() = default;
    ~CVulkanQueue();

    void Init(VkDevice device, VkSwapchainKHR swapchain, uint32_t familyIndex,
              uint32_t queueIndex);
    void Destroy();
    std::optional<uint32_t> AcquireNextImage();
    void SubmitSync(VkCommandBuffer commandBuffer);
    void SubmitAsync(VkCommandBuffer commandBuffer);
    void Present(uint32_t imageIndex);
    void WaitIdle();

    VkQueue GetHandle() const;
    uint32_t GetFamilyIndex() const;

private:
    uint32_t mFamilyIndex = 0;
    VkDevice mDevice = VK_NULL_HANDLE;
    VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
    VkQueue mQueue = VK_NULL_HANDLE;
    VkSemaphore mImageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore mRenderFinishedSemaphore = VK_NULL_HANDLE;
};
} // namespace Renderer