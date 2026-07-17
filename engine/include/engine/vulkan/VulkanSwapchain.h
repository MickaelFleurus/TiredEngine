#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace Vulkan {
struct SContext;

class CSwapchain {
public:
    CSwapchain(SContext& context);
    ~CSwapchain();
    void Recreate();

    VkCommandPool GetCommandPool() const;
    VkSwapchainKHR GetSwapchain() const;
    VkFormat GetImageFormat() const;
    const VkExtent2D& GetExtent() const;
    const std::vector<VkImage>& GetImages() const;
    const std::vector<VkImageView>& GetImageViews() const;
    VkRenderPass GetRenderPass() const;
    int GetImagesCount() const;
    const std::vector<VkCommandBuffer>& GetCommandBuffers() const;
    VkCommandBuffer GetCommandBuffer(int index) const;
    const std::vector<VkFramebuffer>& GetFramebuffers() const;
    VkFramebuffer GetFramebuffer(int index) const;

private:
    SContext& mContext;

    VkCommandPool mCommandPool;
    VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
    VkFormat mImageFormat;
    VkExtent2D mExtent;
    std::vector<VkImage> mImages;
    std::vector<VkImageView> mImageViews;

    // Rendering
    VkRenderPass mRenderPass = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> mCommandBuffers;
    std::vector<VkFramebuffer> mFramebuffers;
};
} // namespace Vulkan
