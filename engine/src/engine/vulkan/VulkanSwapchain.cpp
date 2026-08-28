#include "engine/vulkan/VulkanSwapchain.h"

#include "engine/utils/Logger.h"
#include "engine/vulkan/VulkanContext.h"
#include "engine/vulkan/VulkanInitializer.h"

namespace {
VkCommandPool CreateCommandBufferPool(VkDevice device,
                                      uint32_t queueFamilyIndex) {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndex;

    VkCommandPool commandPool;
    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) !=
        VK_SUCCESS) {
        LOG_FATAL("Failed to create command pool!");
    }
    return commandPool;
}
} // namespace

namespace Vulkan {

CSwapchain::CSwapchain(SContext& context)
    : mContext(context)
    , mCommandPool(CreateCommandBufferPool(mContext.device,
                                           mContext.graphicsQueueFamilyIndex)) {
}

CSwapchain::~CSwapchain() {
    vkDestroyCommandPool(mContext.device, mCommandPool, nullptr);
    for (auto framebuffer : mFramebuffers) {
        vkDestroyFramebuffer(mContext.device, framebuffer, nullptr);
    }
    if (mRenderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(mContext.device, mRenderPass, nullptr);
    }
    for (auto imageView : mImageViews) {
        vkDestroyImageView(mContext.device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(mContext.device, mSwapchain, nullptr);
}

VkCommandPool CSwapchain::GetCommandPool() const {
    return mCommandPool;
}

VkSwapchainKHR CSwapchain::GetSwapchain() const {
    return mSwapchain;
}

VkFormat CSwapchain::GetImageFormat() const {
    return mImageFormat;
}

const VkExtent2D& CSwapchain::GetExtent() const {
    return mExtent;
}

const std::vector<VkImage>& CSwapchain::GetImages() const {
    return mImages;
}

const std::vector<VkImageView>& CSwapchain::GetImageViews() const {
    return mImageViews;
}

VkRenderPass CSwapchain::GetRenderPass() const {
    return mRenderPass;
}

int CSwapchain::GetImagesCount() const {
    return mImages.size();
}

const std::vector<VkCommandBuffer>& CSwapchain::GetCommandBuffers() const {
    return mCommandBuffers;
}

const std::vector<VkFramebuffer>& CSwapchain::GetFramebuffers() const {
    return mFramebuffers;
}

VkCommandBuffer CSwapchain::GetCommandBuffer(int index) const {
    if (index < 0 || index >= mCommandBuffers.size()) {
        LOG_FATAL("Invalid command buffer index: {}", index);
    }
    return mCommandBuffers[index];
}

VkFramebuffer CSwapchain::GetFramebuffer(int index) const {
    if (index < 0 || index >= mFramebuffers.size()) {
        LOG_FATAL("Invalid framebuffer index: {}", index);
    }
    return mFramebuffers[index];
}

void CSwapchain::Recreate() {
    vkDeviceWaitIdle(mContext.device);

    for (auto framebuffer : mFramebuffers) {
        vkDestroyFramebuffer(mContext.device, framebuffer, nullptr);
    }
    mFramebuffers.clear();

    if (mRenderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(mContext.device, mRenderPass, nullptr);
    }

    for (auto imageView : mImageViews) {
        vkDestroyImageView(mContext.device, imageView, nullptr);
    }
    mImageViews.clear();

    if (mSwapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(mContext.device, mSwapchain, nullptr);
    }

    auto [swapchain, imageFormat, extent, images, imageViews] = CreateSwapchain(
        mContext.physicalDevice, mContext.device, mContext.surface,
        mContext.window.get(), mContext.graphicsQueueFamilyIndex,
        mContext.presentQueueFamilyIndex);

    mSwapchain = swapchain;
    mImageFormat = imageFormat;
    mExtent = extent;
    mImages = std::move(images);
    mImageViews = std::move(imageViews);

    mRenderPass = CreateRenderPass(mContext.device, mImageFormat);

    mCommandBuffers = std::move(CreateCommandBuffers(
        mContext.device, mCommandPool, mImageViews.size()));

    mFramebuffers = std::move(
        CreateFramebuffers(mContext.device, mRenderPass, mImageViews, mExtent));
    mDepthBuffer = std::move(
        CreateDepthBuffer(mContext.vmaAllocator, mContext.device, mExtent));
}

} // namespace Vulkan
