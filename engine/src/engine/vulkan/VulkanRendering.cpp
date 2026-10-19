#include "engine/vulkan/VulkanRendering.h"

#include "engine/utils/Logger.h"

namespace {
void transitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout oldL,
                     VkImageLayout newL, VkPipelineStageFlags2 srcStage,
                     VkAccessFlags2 srcAccess, VkPipelineStageFlags2 dstStage,
                     VkAccessFlags2 dstAccess,
                     VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT) {
    VkImageMemoryBarrier2 b{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
    b.srcStageMask = srcStage;
    b.srcAccessMask = srcAccess;
    b.dstStageMask = dstStage;
    b.dstAccessMask = dstAccess;
    b.oldLayout = oldL;
    b.newLayout = newL;
    b.srcQueueFamilyIndex = b.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    b.image = image;
    b.subresourceRange = {aspect, 0, 1, 0, 1};

    VkDependencyInfo dep{VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
    dep.imageMemoryBarrierCount = 1;
    dep.pImageMemoryBarriers = &b;
    vkCmdPipelineBarrier2(cmd, &dep);
}
} // namespace

namespace Vulkan {

CVulkanRendering::CVulkanRendering(const SContext& context,
                                   CSwapchain& swapchain)
    : mContext(context), mSwapchain(swapchain) {
    VkDevice device = mContext.device;
    vkGetDeviceQueue(device, mContext.graphicsQueueFamilyIndex, 0, &mQueue);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                          &mImageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                          &mRenderFinishedSemaphore) != VK_SUCCESS) {
        LOG_FATAL("Failed to create semaphores!");
    }

    // Create fence
    for (int i = 0; i < mSwapchain.GetImagesCount(); ++i) {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VkFence fence;
        if (vkCreateFence(device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
            LOG_FATAL("Failed to create fence!");
        }
        mFrameFences.push_back(fence);
    }
}

CVulkanRendering::~CVulkanRendering() {
    Destroy();
}

void CVulkanRendering::Destroy() {
    VkDevice device = mContext.device;
    WaitIdle();
    vkDestroySemaphore(device, mRenderFinishedSemaphore, nullptr);
    vkDestroySemaphore(device, mImageAvailableSemaphore, nullptr);
    for (auto& fence : mFrameFences) {
        vkDestroyFence(device, fence, nullptr);
    }
}

std::optional<uint32_t> CVulkanRendering::AcquireNextImage() {
    uint32_t imageIndex;
    if (auto result = vkAcquireNextImageKHR(
            mContext.device, mSwapchain.GetSwapchain(), UINT64_MAX,
            mImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        result != VK_SUCCESS) {
        mSwapchain.Recreate();
        if (auto result = vkAcquireNextImageKHR(
                mContext.device, mSwapchain.GetSwapchain(), UINT64_MAX,
                mImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
            result != VK_SUCCESS) {
            LOG_ERROR("Failed to acquire swapchain image after recreation!");
            return std::nullopt;
        }
    }
    return imageIndex;
}

void CVulkanRendering::SubmitSync(VkCommandBuffer commandBuffer,
                                  uint32_t imageIndex) const {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkDevice device = mContext.device;
    vkResetFences(device, 1, &mFrameFences[imageIndex]);
    if (vkQueueSubmit(mQueue, 1, &submitInfo, mFrameFences[imageIndex]) !=
        VK_SUCCESS) {
        LOG_ERROR("Failed to submit command buffer!");
    }
    vkWaitForFences(device, 1, &mFrameFences[imageIndex], VK_TRUE, UINT64_MAX);
}

void CVulkanRendering::SubmitAsync(VkCommandBuffer commandBuffer,
                                   uint32_t imageIndex) const {
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.pSignalSemaphores = &mRenderFinishedSemaphore;
    submitInfo.signalSemaphoreCount = 1;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    submitInfo.pWaitSemaphores = &mImageAvailableSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.waitSemaphoreCount = 1;
    if (vkQueueSubmit(mQueue, 1, &submitInfo, mFrameFences[imageIndex]) !=
        VK_SUCCESS) {
        LOG_ERROR("Failed to submit command buffer!");
    }
}

void CVulkanRendering::SubmitSyncSingleUse(
    VkCommandBuffer commandBuffer) const {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    // Use a temporary fence or just wait on the queue
    if (vkQueueSubmit(mQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        LOG_ERROR("Failed to submit command buffer!");
    }

    // Wait for queue to finish (simpler than managing a fence)
    vkQueueWaitIdle(mQueue);
}

void CVulkanRendering::Present(uint32_t imageIndex) {
    VkSwapchainKHR swapchain = mSwapchain.GetSwapchain();
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &mRenderFinishedSemaphore;

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &imageIndex;

    VkResult result = vkQueuePresentKHR(mQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        mSwapchain.Recreate();
    } else if (result != VK_SUCCESS) {
        LOG_FATAL("Failed to present swapchain image: {}",
                  static_cast<int>(result));
    }
}

void CVulkanRendering::WaitIdle() const {
    vkQueueWaitIdle(mQueue);
}

void CVulkanRendering::BeginRenderPass(uint32_t index, VkViewport viewport,
                                       VkRect2D scissor) {

    VkImage swapImage = mSwapchain.GetImages()[index];
    VkImageView depthImage = mSwapchain.GetDepthView();
    VkImageView swapView = mSwapchain.GetImageViews()[index];

    VkDevice device = mContext.device;

    // Wait for GPU to finish with this frame
    vkWaitForFences(device, 1, &mFrameFences[index], VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &mFrameFences[index]);

    VkCommandBuffer commandBuffer = mSwapchain.GetCommandBuffer(index);
    vkResetCommandBuffer(commandBuffer, 0);

    VkRenderingAttachmentInfo color{
        VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
    color.imageView = mSwapchain.GetImageViews()[index];
    color.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color.clearValue = {{0.f, 0.f, 0.f, 1.f}};

    VkRenderingAttachmentInfo depth{
        VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
    depth.imageView = mSwapchain.GetDepthView();
    depth.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    depth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth.clearValue.depthStencil = {1.f, 0};

    VkRenderingInfo info{VK_STRUCTURE_TYPE_RENDERING_INFO};
    info.renderArea = {{0, 0}, mSwapchain.GetExtent()};
    info.layerCount = 1;
    info.colorAttachmentCount = 1;
    info.pColorAttachments = &color;
    info.pDepthAttachment = &depth;

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        LOG_ERROR("Failed to begin recording command buffer number {}!", index);
    }

    transitionImage(commandBuffer, swapImage, VK_IMAGE_LAYOUT_UNDEFINED,
                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, 0,
                    VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                    VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);

    // (B) depth: UNDEFINED -> DEPTH_ATTACHMENT_OPTIMAL
    // transitionImage(commandBuffer, depthImage, VK_IMAGE_LAYOUT_UNDEFINED,
    //                 VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
    //                 VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
    //                     VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
    //                 0,
    //                 VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
    //                     VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
    //                 VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    //                 VK_IMAGE_ASPECT_DEPTH_BIT);

    vkCmdBeginRendering(commandBuffer, &info);
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void CVulkanRendering::EndRenderPass(uint32_t index) {
    VkCommandBuffer commandBuffer = mSwapchain.GetCommandBuffer(index);

    vkCmdEndRendering(commandBuffer);

    transitionImage(commandBuffer, mSwapchain.GetImages()[index],
                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                    VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                    VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                    VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT, 0);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        LOG_ERROR("Failed to record command buffer number {}!", index);
    }
}

} // namespace Vulkan
