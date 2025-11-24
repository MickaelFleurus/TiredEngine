#pragma once

#include "engine/vulkan/IVulkanContext.h"

namespace Vulkan {
class CVulkanContext : public IVulkanContextGetter,
                       public IVulkanContextSetter {
public:
    CVulkanContext() = default;
    ~CVulkanContext();

    VkInstance GetInstance() const override;
    VkPhysicalDevice GetPhysicalDevice() const override;
    VkDevice GetDevice() const override;
    VkQueue GetGraphicsQueue() const override;
    VkQueue GetPresentQueue() const override;
    uint32_t GetGraphicsQueueFamilyIndex() const override;
    uint32_t GetPresentQueueFamilyIndex() const override;
    VkSurfaceKHR GetSurface() const override;
    VkCommandBuffer GetCommandBuffer(uint32_t imageIndex) override;
    VkSwapchainKHR GetSwapchain() const override;
    VkFramebuffer GetFramebuffer(uint32_t imageIndex) override;
    VkRenderPass GetRenderPass() const override;
    VkPhysicalDeviceMemoryProperties
    GetPhysicalDeviceMemoryProperties() const override;
    VkCommandPool GetCommandPool() const override;

    void SetMemoryAllocator(CMemoryAllocator* allocator) override;
    void SetBufferHandler(CBufferHandler* bufferHandler) override;
    void SetDebugMessenger(VkDebugUtilsMessengerEXT debugMessenger) override;

    void SetInstance(VkInstance instance) override;
    void SetPhysicalDevice(VkPhysicalDevice physicalDevice) override;
    void
    SetPhysicalDeviceProperties(VkPhysicalDeviceProperties properties) override;
    void SetPhysicalDeviceFeatures(VkPhysicalDeviceFeatures features) override;
    void SetPhysicalDeviceMemoryProperties(
        VkPhysicalDeviceMemoryProperties memoryProperties) override;
    void SetDevice(VkDevice device) override;
    void SetGraphicsQueue(VkQueue graphicsQueue) override;
    void SetPresentQueue(VkQueue presentQueue) override;
    void
    SetGraphicsQueueFamilyIndex(uint32_t graphicsQueueFamilyIndex) override;
    void SetPresentQueueFamilyIndex(uint32_t presentQueueFamilyIndex) override;
    void SetSurface(VkSurfaceKHR surface) override;
    void SetSwapchain(VkSwapchainKHR swapchain) override;
    void SetSwapchainImageFormat(VkFormat imageFormat) override;
    void SetSwapchainExtent(VkExtent2D extent) override;
    void SetSwapchainImages(std::vector<VkImage> images) override;
    void SetSwapchainImageViews(std::vector<VkImageView> imageViews) override;
    void SetRenderPass(VkRenderPass renderPass) override;
    void SetCommandPool(VkCommandPool commandPool) override;
    void
    SetCommandBuffers(std::vector<VkCommandBuffer> commandBuffers) override;
    void SetFramebuffers(std::vector<VkFramebuffer> framebuffers) override;

private:
    VkInstance mInstance;
    VkSurfaceKHR mSurface;
    VkDebugUtilsMessengerEXT mDebugMessenger;

    // Logical device
    VkDevice mDevice;
    VkQueue mGraphicsQueue;
    VkQueue mPresentQueue;
    uint32_t mGraphicsQueueFamilyIndex;
    uint32_t mPresentQueueFamilyIndex;

    // Physical device
    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties mPhysicalDeviceProperties;
    VkPhysicalDeviceFeatures mPhysicalDeviceFeatures;
    VkPhysicalDeviceMemoryProperties mPhysicalDeviceMemoryProperties;

    // Swap chain
    VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
    VkFormat mImageFormat;
    VkExtent2D mExtent;
    std::vector<VkImage> mImages;
    std::vector<VkImageView> mImageViews;

    // Rendering
    VkRenderPass mRenderPass;
    int mImagesCount = 0;
    VkCommandPool mCommandPool;
    std::vector<VkCommandBuffer> mCommandBuffers;
    std::vector<VkFramebuffer> mFramebuffers;
};
} // namespace Vulkan