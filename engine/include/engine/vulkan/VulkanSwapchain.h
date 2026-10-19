#pragma once

#include <vector>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace Vulkan {
struct SContext;

class CSwapchain {
public:
    struct SDepthBuffer {
        VkImage image = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE;
        VkFormat format = VK_FORMAT_UNDEFINED;
    };

    explicit CSwapchain(SContext& context);
    ~CSwapchain();
    void Recreate();

    VkCommandPool GetCommandPool() const;
    VkSwapchainKHR GetSwapchain() const;
    VkFormat GetImageFormat() const;
    const VkExtent2D& GetExtent() const;
    const std::vector<VkImage>& GetImages() const;
    const std::vector<VkImageView>& GetImageViews() const;
    VkImageView GetDepthView() const;
    VkFormat GetDepthFormat() const;
    int GetImagesCount() const;
    const std::vector<VkCommandBuffer>& GetCommandBuffers() const;
    VkCommandBuffer GetCommandBuffer(int index) const;

private:
    SContext& mContext;

    VkCommandPool mCommandPool;
    VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
    VkFormat mImageFormat;
    VkExtent2D mExtent;
    std::vector<VkImage> mImages;
    std::vector<VkImageView> mImageViews;
    SDepthBuffer mDepthBuffer{};

    // Rendering
    std::vector<VkCommandBuffer> mCommandBuffers;
};
} // namespace Vulkan
