#include "engine/renderer/VulkanQueue.h"

#include "engine/utils/Logger.h"

namespace Renderer {

CVulkanQueue::~CVulkanQueue() {
    Destroy();
}

void CVulkanQueue::Init(VkDevice device, VkSwapchainKHR swapchain,
                        uint32_t familyIndex, uint32_t queueIndex) {
    mDevice = device;
    mSwapchain = swapchain;
    mFamilyIndex = familyIndex;

    vkGetDeviceQueue(mDevice, familyIndex, queueIndex, &mQueue);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr,
                          &mImageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr,
                          &mRenderFinishedSemaphore) != VK_SUCCESS) {
        LOG_FATAL("Failed to create semaphores!");
    }
}

void CVulkanQueue::Destroy() {
    if (mDevice == VK_NULL_HANDLE) {
        return;
    }
    WaitIdle();
    vkDestroySemaphore(mDevice, mRenderFinishedSemaphore, nullptr);
    vkDestroySemaphore(mDevice, mImageAvailableSemaphore, nullptr);
    mDevice = VK_NULL_HANDLE;
    mSwapchain = VK_NULL_HANDLE;
}

std::optional<uint32_t> CVulkanQueue::AcquireNextImage() {
    uint32_t imageIndex;
    if (vkAcquireNextImageKHR(mDevice, mSwapchain, UINT64_MAX,
                              mImageAvailableSemaphore, VK_NULL_HANDLE,
                              &imageIndex) != VK_SUCCESS) {
        LOG_ERROR("Failed to acquire next image!");
        return std::nullopt;
    }
    return imageIndex;
}

void CVulkanQueue::SubmitSync(VkCommandBuffer commandBuffer) const {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    if (vkQueueSubmit(mQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        LOG_ERROR("Failed to submit command buffer!");
    }
}

void CVulkanQueue::SubmitAsync(VkCommandBuffer commandBuffer) const {
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

void CVulkanQueue::Present(uint32_t imageIndex) {
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &mRenderFinishedSemaphore;

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &mSwapchain;
    presentInfo.pImageIndices = &imageIndex;

    if (vkQueuePresentKHR(mQueue, &presentInfo) != VK_SUCCESS) {
        LOG_FATAL("Failed to present swapchain image!");
    }
}

void CVulkanQueue::WaitIdle() const {
    vkQueueWaitIdle(mQueue);
}

VkQueue CVulkanQueue::GetHandle() const {
    return mQueue;
}

uint32_t CVulkanQueue::GetFamilyIndex() const {
    return mFamilyIndex;
}

} // namespace Renderer