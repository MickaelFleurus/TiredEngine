#include "engine/vulkan/ViewportTarget.h"

#include <algorithm>

#include "engine/utils/Logger.h"
#include "engine/vulkan/VulkanContext.h"
#include "engine/vulkan/VulkanRendering.h"
#include "engine/vulkan/VulkanSwapchain.h"

namespace Vulkan {
CViewportTarget::CViewportTarget(const SContext& context, CSwapchain& swapchain,
                                 CVulkanRendering& rendering)
    : mContext(context), mSwapchain(swapchain), mRendering(rendering) {
    CreateRenderPass();

    VkCommandBufferAllocateInfo allocateInfo{
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    allocateInfo.commandPool = mSwapchain.GetCommandPool();
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;
    if (vkAllocateCommandBuffers(mContext.device, &allocateInfo,
                                 &mCommandBuffer) != VK_SUCCESS) {
        LOG_FATAL("Failed to allocate viewport command buffer");
    }

    const auto& extent = mSwapchain.GetExtent();
    CreateTargetImages(extent.width, extent.height);
}

CViewportTarget::~CViewportTarget() {
    mRendering.WaitIdle();
    DestroyTargetImages();
    if (mRenderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(mContext.device, mRenderPass, nullptr);
    }
    if (mCommandBuffer != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(mContext.device, mSwapchain.GetCommandPool(), 1,
                             &mCommandBuffer);
    }
}

void CViewportTarget::RequestResize(uint32_t width, uint32_t height) {
    if (width == 0 || height == 0) {
        return;
    }
    const uint32_t maxExtent =
        mContext.physicalDeviceProperties.limits.maxImageDimension2D;
    VkExtent2D requested{std::min(width, maxExtent),
                         std::min(height, maxExtent)};
    if (requested.width != mExtent.width ||
        requested.height != mExtent.height) {
        mPendingExtent = requested;
    }
}

void CViewportTarget::ApplyPendingResize() {
    if (!mPendingExtent) {
        return;
    }
    mRendering.WaitIdle();
    DestroyTargetImages();
    CreateTargetImages(mPendingExtent->width, mPendingExtent->height);
    mPendingExtent.reset();
}

VkCommandBuffer CViewportTarget::BeginRender() {
    vkResetCommandBuffer(mCommandBuffer, 0);
    VkCommandBufferBeginInfo beginInfo{
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if (vkBeginCommandBuffer(mCommandBuffer, &beginInfo) != VK_SUCCESS) {
        LOG_FATAL("Failed to begin viewport command buffer");
        return VK_NULL_HANDLE;
    }

    VkClearValue clearValues[2]{};
    clearValues[0].color = {{0.035f, 0.04f, 0.045f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};
    VkRenderPassBeginInfo renderPassInfo{
        VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    renderPassInfo.renderPass = mRenderPass;
    renderPassInfo.framebuffer = mFramebuffer;
    renderPassInfo.renderArea = {{0, 0}, mExtent};
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearValues;
    vkCmdBeginRenderPass(mCommandBuffer, &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.width = static_cast<float>(mExtent.width);
    viewport.height = static_cast<float>(mExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, mExtent};
    vkCmdSetViewport(mCommandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(mCommandBuffer, 0, 1, &scissor);
    return mCommandBuffer;
}

void CViewportTarget::EndRender() {
    vkCmdEndRenderPass(mCommandBuffer);
    if (vkEndCommandBuffer(mCommandBuffer) != VK_SUCCESS) {
        LOG_FATAL("Failed to end viewport command buffer");
        return;
    }
    mRendering.SubmitSyncSingleUse(mCommandBuffer);
}

VkImageView CViewportTarget::GetImageView() const {
    return mImageView;
}

VkRenderPass CViewportTarget::GetRenderPass() const {
    return mRenderPass;
}

const VkExtent2D& CViewportTarget::GetExtent() const {
    return mExtent;
}

void CViewportTarget::CreateRenderPass() {
    VkAttachmentDescription attachment{};
    attachment.format = mSwapchain.GetImageFormat();
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = mSwapchain.GetDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorReference{};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkAttachmentReference depthReference{};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorReference;
    subpass.pDepthStencilAttachment = &depthReference;

    VkSubpassDependency dependencies[2]{};
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[0].dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[0].dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
        VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[1].srcAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    VkRenderPassCreateInfo renderPassInfo{
        VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    VkAttachmentDescription attachments[] = {attachment, depthAttachment};
    renderPassInfo.attachmentCount = 2;
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 2;
    renderPassInfo.pDependencies = dependencies;
    if (vkCreateRenderPass(mContext.device, &renderPassInfo, nullptr,
                           &mRenderPass) != VK_SUCCESS) {
        LOG_FATAL("Failed to create viewport render pass");
    }
}

void CViewportTarget::CreateTargetImages(uint32_t width, uint32_t height) {
    VkImageCreateInfo imageInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = mSwapchain.GetImageFormat();
    imageInfo.extent = {width, height, 1};
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VmaAllocationCreateInfo allocationInfo{};
    allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    if (vmaCreateImage(mContext.vmaAllocator, &imageInfo, &allocationInfo,
                       &mImage, &mAllocation, nullptr) != VK_SUCCESS) {
        LOG_FATAL("Failed to create viewport image");
        return;
    }

    VkImageViewCreateInfo viewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    viewInfo.image = mImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = imageInfo.format;
    viewInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    if (vkCreateImageView(mContext.device, &viewInfo, nullptr, &mImageView) !=
        VK_SUCCESS) {
        LOG_FATAL("Failed to create viewport image view");
        return;
    }

    VkImageCreateInfo depthInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    depthInfo.imageType = VK_IMAGE_TYPE_2D;
    depthInfo.format = mSwapchain.GetDepthFormat();
    depthInfo.extent = {width, height, 1};
    depthInfo.mipLevels = 1;
    depthInfo.arrayLayers = 1;
    depthInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    depthInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    depthInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    depthInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    depthInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    if (vmaCreateImage(mContext.vmaAllocator, &depthInfo, &allocationInfo,
                       &mDepthImage, &mDepthAllocation,
                       nullptr) != VK_SUCCESS) {
        LOG_FATAL("Failed to create viewport depth image");
        return;
    }

    VkImageViewCreateInfo depthViewInfo{
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    depthViewInfo.image = mDepthImage;
    depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depthViewInfo.format = depthInfo.format;
    depthViewInfo.subresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1};
    if (vkCreateImageView(mContext.device, &depthViewInfo, nullptr,
                          &mDepthView) != VK_SUCCESS) {
        LOG_FATAL("Failed to create viewport depth image view");
        return;
    }

    VkFramebufferCreateInfo framebufferInfo{
        VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    framebufferInfo.renderPass = mRenderPass;
    VkImageView framebufferAttachments[] = {mImageView, mDepthView};
    framebufferInfo.attachmentCount = 2;
    framebufferInfo.pAttachments = framebufferAttachments;
    framebufferInfo.width = width;
    framebufferInfo.height = height;
    framebufferInfo.layers = 1;
    if (vkCreateFramebuffer(mContext.device, &framebufferInfo, nullptr,
                            &mFramebuffer) != VK_SUCCESS) {
        LOG_FATAL("Failed to create viewport framebuffer");
        return;
    }
    mExtent = {width, height};
}

void CViewportTarget::DestroyTargetImages() {
    if (mFramebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(mContext.device, mFramebuffer, nullptr);
        mFramebuffer = VK_NULL_HANDLE;
    }
    if (mImageView != VK_NULL_HANDLE) {
        vkDestroyImageView(mContext.device, mImageView, nullptr);
        mImageView = VK_NULL_HANDLE;
    }
    if (mDepthView != VK_NULL_HANDLE) {
        vkDestroyImageView(mContext.device, mDepthView, nullptr);
        mDepthView = VK_NULL_HANDLE;
    }
    if (mImage != VK_NULL_HANDLE) {
        vmaDestroyImage(mContext.vmaAllocator, mImage, mAllocation);
        mImage = VK_NULL_HANDLE;
        mAllocation = VK_NULL_HANDLE;
    }
    if (mDepthImage != VK_NULL_HANDLE) {
        vmaDestroyImage(mContext.vmaAllocator, mDepthImage, mDepthAllocation);
        mDepthImage = VK_NULL_HANDLE;
        mDepthAllocation = VK_NULL_HANDLE;
    }
}
} // namespace Vulkan
