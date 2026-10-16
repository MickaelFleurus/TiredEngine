#pragma once

#include <optional>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace Vulkan {
struct SContext;
class CSwapchain;
class CVulkanRendering;

class CViewportTarget {
public:
    CViewportTarget(const SContext& context, CSwapchain& swapchain,
                    CVulkanRendering& rendering);
    ~CViewportTarget();

    CViewportTarget(const CViewportTarget&) = delete;
    CViewportTarget& operator=(const CViewportTarget&) = delete;

    void RequestResize(uint32_t width, uint32_t height);
    void ApplyPendingResize();
    VkCommandBuffer BeginRender();
    void EndRender();

    VkImageView GetImageView() const;
    const VkExtent2D& GetExtent() const;

private:
    void CreateRenderPass();
    void CreateTargetImages(uint32_t width, uint32_t height);
    void DestroyTargetImages();

    const SContext& mContext;
    CSwapchain& mSwapchain;
    CVulkanRendering& mRendering;
    VkRenderPass mRenderPass = VK_NULL_HANDLE;
    VkImage mImage = VK_NULL_HANDLE;
    VmaAllocation mAllocation = VK_NULL_HANDLE;
    VkImageView mImageView = VK_NULL_HANDLE;
    VkImage mDepthImage = VK_NULL_HANDLE;
    VmaAllocation mDepthAllocation = VK_NULL_HANDLE;
    VkImageView mDepthView = VK_NULL_HANDLE;
    VkFramebuffer mFramebuffer = VK_NULL_HANDLE;
    VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;
    VkExtent2D mExtent{};
    std::optional<VkExtent2D> mPendingExtent;
};
} // namespace Vulkan
