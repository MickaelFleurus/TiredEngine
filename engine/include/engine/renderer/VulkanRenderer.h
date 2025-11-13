#pragma once
#include "engine/renderer/VulkanQueue.h"
#include "engine/utils/VulkanUtils.h"

#include <vector>

struct SDL_Window;

namespace System {
class CSystem;
}

namespace Renderer {
class VulkanRenderer {
public:
    explicit VulkanRenderer();
    ~VulkanRenderer();

    bool Init(SDL_Window* window, const System::CSystem& system);

    VkCommandBuffer GetCommandBuffer(uint32_t imageIndex);
    CVulkanQueue& GetQueue();
    const CVulkanQueue& GetQueue() const;
    const Utils::VulkanDevice& GetVulkanDevice() const;
    const Utils::VulkanPhysicalDevice& GetVulkanPhysicalDevice() const;
    VkRenderPass GetRenderPass() const;
    VkInstance GetInstance() const;
    VkPhysicalDevice GetPhysicalDevice() const;
    VkDevice GetDevice() const;
    const Utils::VulkanSwapchain& GetSwapchain() const;

    void BeginRenderPass(uint32_t index, VkViewport viewport, VkRect2D scissor);

    void EndRenderPass(uint32_t index);

private:
    VkInstance mInstance;
    VkSurfaceKHR mSurface;
    VkDebugUtilsMessengerEXT mDebugMessenger;
    Utils::VulkanPhysicalDevice mPhysicalDevice;
    Utils::VulkanDevice mVulkanDevice;
    Utils::SwapChainSupportDetails mSwapChainSupport;
    Utils::VulkanSwapchain mSwapchain;
    VkRenderPass mRenderPass;
    CVulkanQueue mQueue;

    VkCommandPool mCommandPool;
    int mImagesCount = 0;
    std::vector<VkCommandBuffer> mCommandBuffers;
    std::vector<VkFramebuffer> mFramebuffers;
};
} // namespace Renderer