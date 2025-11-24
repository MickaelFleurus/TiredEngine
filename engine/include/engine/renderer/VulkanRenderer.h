#pragma once
#include "engine/renderer/VulkanQueue.h"
#include "engine/utils/VulkanUtils.h"

#include <vector>

struct SDL_Window;

namespace System {
class CSystem;
}

namespace Renderer {
class CVulkanRenderer {
public:
    explicit CVulkanRenderer();
    ~CVulkanRenderer();

    bool Init(SDL_Window* window, const System::CSystem& system);

    VkCommandBuffer GetCommandBuffer(uint32_t imageIndex);
    CVulkanRendering& GetQueue();
    const CVulkanRendering& GetQueue() const;
    const Utils::VulkanDevice& GetVulkanDevice() const;
    const Utils::VulkanPhysicalDevice& GetVulkanPhysicalDevice() const;
    VkRenderPass GetRenderPass() const;
    VkInstance GetInstance() const;
    VkPhysicalDevice GetPhysicalDevice() const;
    VkDevice GetDevice() const;
    const Utils::VulkanSwapchain& GetSwapchain() const;
    VkDescriptorPool GetDescriptorPool() const;

    void BeginRenderPass(uint32_t index, VkViewport viewport, VkRect2D scissor);
    void EndRenderPass(uint32_t index);

    VkCommandBuffer BeginSingleTimeCommands() const;
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;

private:
    VkInstance mInstance;
    VkSurfaceKHR mSurface;
    VkDebugUtilsMessengerEXT mDebugMessenger;
    Utils::VulkanPhysicalDevice mPhysicalDevice;
    Utils::VulkanDevice mVulkanDevice;
    Utils::SwapChainSupportDetails mSwapChainSupport;
    Utils::VulkanSwapchain mSwapchain;
    VkRenderPass mRenderPass;
    CVulkanRendering mQueue;

    VkCommandPool mCommandPool;
    VkDescriptorPool mDescriptorPool;
    int mImagesCount = 0;
    std::vector<VkCommandBuffer> mCommandBuffers;
    std::vector<VkFramebuffer> mFramebuffers;
};
} // namespace Renderer