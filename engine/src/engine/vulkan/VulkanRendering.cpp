#include "engine/vulkan/VulkanRendering.h"

#include "engine/utils/Logger.h"

namespace Vulkan {

CVulkanRendering::CVulkanRendering(CVulkanContext& vulkanContext)
    : mVulkanContext(vulkanContext) {
    VkDevice device = mVulkanContext.GetDevice();
    vkGetDeviceQueue(device, mVulkanContext.GetGraphicsQueueFamilyIndex(),
                     mVulkanContext.GetGraphicsQueueFamilyIndex(), &mQueue);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                          &mImageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                          &mRenderFinishedSemaphore) != VK_SUCCESS) {
        LOG_FATAL("Failed to create semaphores!");
    }
}

CVulkanRendering::~CVulkanRendering() {
    Destroy();
}

void CVulkanRendering::Destroy() {
    VkDevice device = mVulkanContext.GetDevice();
    WaitIdle();
    vkDestroySemaphore(device, mRenderFinishedSemaphore, nullptr);
    vkDestroySemaphore(device, mImageAvailableSemaphore, nullptr);
}

std::optional<uint32_t> CVulkanRendering::AcquireNextImage() {
    uint32_t imageIndex;
    if (auto result = vkAcquireNextImageKHR(
            mVulkanContext.GetDevice(), mVulkanContext.GetSwapchain(),
            UINT64_MAX, mImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        result != VK_SUCCESS) {
        mVulkanContext.RecreateSwapchainResources();
        if (auto result = vkAcquireNextImageKHR(
                mVulkanContext.GetDevice(), mVulkanContext.GetSwapchain(),
                UINT64_MAX, mImageAvailableSemaphore, VK_NULL_HANDLE,
                &imageIndex);
            result != VK_SUCCESS) {
            LOG_ERROR("Failed to acquire swapchain image after recreation!");
            return std::nullopt;
        }
    }
    return imageIndex;
}

void CVulkanRendering::SubmitSync(VkCommandBuffer commandBuffer) const {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    if (vkQueueSubmit(mQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        LOG_ERROR("Failed to submit command buffer!");
    }
}

void CVulkanRendering::SubmitAsync(VkCommandBuffer commandBuffer) const {
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
    if (vkQueueSubmit(mQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        LOG_ERROR("Failed to submit command buffer!");
    }
}

void CVulkanRendering::Present(uint32_t imageIndex) {
    VkSwapchainKHR swapchain = mVulkanContext.GetSwapchain();
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &mRenderFinishedSemaphore;

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &imageIndex;

    if (vkQueuePresentKHR(mQueue, &presentInfo) != VK_SUCCESS) {
        LOG_FATAL("Failed to present swapchain image!");
    }
}

void CVulkanRendering::WaitIdle() const {
    vkQueueWaitIdle(mQueue);
}

VkQueue CVulkanRendering::GetHandle() const {
    return mQueue;
}

void CVulkanRendering::BeginRenderPass(uint32_t index, VkViewport viewport,
                                       VkRect2D scissor) {

    VkCommandBuffer commandBuffer = mVulkanContext.GetCommandBuffer(index);

    VkClearColorValue red{0.0f, 0.0f, 0.0f, 0.0f};
    VkClearValue clearColor{};
    clearColor.color = red;
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = mVulkanContext.GetRenderPass();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = mVulkanContext.GetSwapchainExtent();
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    renderPassInfo.framebuffer = mVulkanContext.GetFramebuffer(index);
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        LOG_ERROR("Failed to begin recording command buffer number {}!", index);
    }

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void CVulkanRendering::EndRenderPass(uint32_t index) {
    VkCommandBuffer commandBuffer = mVulkanContext.GetCommandBuffer(index);
    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        LOG_ERROR("Failed to record command buffer number {}!", index);
    }
}

} // namespace Vulkan