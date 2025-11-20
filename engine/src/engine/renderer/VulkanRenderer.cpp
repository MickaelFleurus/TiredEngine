#include "engine/renderer/VulkanRenderer.h"

#include "engine/system/System.h"

#include "engine/utils/Logger.h"

namespace Renderer {
VulkanRenderer::VulkanRenderer() = default;

VulkanRenderer::~VulkanRenderer() {
    mQueue.Destroy();
    vkDestroyCommandPool(mVulkanDevice.device, mCommandPool, nullptr);
    for (auto framebuffer : mFramebuffers) {
        vkDestroyFramebuffer(mVulkanDevice.device, framebuffer, nullptr);
    }
    vkDestroyRenderPass(mVulkanDevice.device, mRenderPass, nullptr);
    for (auto imageView : mSwapchain.imageViews) {
        vkDestroyImageView(mVulkanDevice.device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(mVulkanDevice.device, mSwapchain.swapchain, nullptr);
    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);

    if (mDebugMessenger != VK_NULL_HANDLE) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            mInstance, "vkDestroyDebugUtilsMessengerEXT");
        if (func)
            func(mInstance, mDebugMessenger, nullptr);
    }
    vkDestroyDevice(mVulkanDevice.device, nullptr);
    vkDestroyInstance(mInstance, nullptr);
}

bool VulkanRenderer::Init(SDL_Window* window, const System::CSystem& system) {
    // Create Vulkan instance
    mInstance = Utils::CreateVulkanInstance(system.GetGameName());

    mDebugMessenger = Utils::CreateVulkanCallback(mInstance);

    // Create surface
    mSurface = Utils::CreateVulkanSurface(window, mInstance);

    // Pick physical device
    mPhysicalDevice = Utils::PickPhysicalDevice(mInstance, mSurface);

    // Create logical device
    mVulkanDevice = Utils::CreateLogicalDevice(mPhysicalDevice, mSurface);

    mSwapChainSupport =
        Utils::QuerySwapChainSupport(mPhysicalDevice.device, mSurface);
    // Create swapchain

    mSwapchain = Utils::CreateSwapchain(
        mPhysicalDevice.device, mVulkanDevice.device, mSurface, window,
        mVulkanDevice.graphicsFamily, mVulkanDevice.presentFamily);

    mImagesCount = static_cast<int>(mSwapchain.images.size());
    mCommandBuffers.resize(mImagesCount);
    mRenderPass =
        Utils::CreateRenderPass(mVulkanDevice.device, mSwapchain.imageFormat);
    Utils::CreateCommandBufferPool(
        mVulkanDevice.device, &mCommandPool,
        mPhysicalDevice.queueFamilies.graphicsFamily.value());
    mQueue.Init(mVulkanDevice.device, mSwapchain.swapchain,
                mVulkanDevice.graphicsFamily, 0);
    mFramebuffers = Utils::CreateFramebuffers(mVulkanDevice.device, mRenderPass,
                                              mSwapchain);

    Utils::CreateCommandBuffers(mVulkanDevice.device, mCommandPool,
                                mCommandBuffers, mImagesCount);

    return true;
}
void VulkanRenderer::BeginRenderPass(uint32_t index, VkViewport viewport,
                                     VkRect2D scissor) {
    VkClearColorValue red{0.0f, 0.0f, 0.0f, 0.0f};
    VkClearValue clearColor{};
    clearColor.color = red;
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = mRenderPass;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = mSwapchain.extent;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    renderPassInfo.framebuffer = mFramebuffers[index];
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;

    if (vkBeginCommandBuffer(mCommandBuffers[index], &beginInfo) !=
        VK_SUCCESS) {
        LOG_ERROR("Failed to begin recording command buffer number {}!", index);
    }

    vkCmdBeginRenderPass(mCommandBuffers[index], &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);
    vkCmdSetViewport(mCommandBuffers[index], 0, 1, &viewport);
    vkCmdSetScissor(mCommandBuffers[index], 0, 1, &scissor);
}

void VulkanRenderer::EndRenderPass(uint32_t index) {
    vkCmdEndRenderPass(mCommandBuffers[index]);

    if (vkEndCommandBuffer(mCommandBuffers[index]) != VK_SUCCESS) {
        LOG_ERROR("Failed to record command buffer number {}!", index);
    }
}

VkCommandBuffer VulkanRenderer::GetCommandBuffer(uint32_t imageIndex) {
    return mCommandBuffers[imageIndex];
}

CVulkanQueue& VulkanRenderer::GetQueue() {
    return mQueue;
}

const CVulkanQueue& VulkanRenderer::GetQueue() const {
    return mQueue;
}

const Utils::VulkanDevice& VulkanRenderer::GetVulkanDevice() const {
    return mVulkanDevice;
}
const Utils::VulkanPhysicalDevice&
VulkanRenderer::GetVulkanPhysicalDevice() const {
    return mPhysicalDevice;
}

VkRenderPass VulkanRenderer::GetRenderPass() const {
    return mRenderPass;
}

VkInstance VulkanRenderer::GetInstance() const {
    return mInstance;
}

VkPhysicalDevice VulkanRenderer::GetPhysicalDevice() const {
    return mPhysicalDevice.device;
}

VkDevice VulkanRenderer::GetDevice() const {
    return mVulkanDevice.device;
}

const Utils::VulkanSwapchain& VulkanRenderer::GetSwapchain() const {
    return mSwapchain;
}

} // namespace Renderer