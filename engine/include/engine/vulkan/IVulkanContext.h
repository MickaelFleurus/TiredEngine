#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace Vulkan {
class CBufferHandler;
class CMemoryAllocator;

class IVulkanContextGetter {
public:
    virtual ~IVulkanContextGetter() = default;

    virtual VkInstance GetInstance() const = 0;
    virtual VkPhysicalDevice GetPhysicalDevice() const = 0;
    virtual VkDevice GetDevice() const = 0;
    virtual VkQueue GetGraphicsQueue() const = 0;
    virtual VkQueue GetPresentQueue() const = 0;
    virtual uint32_t GetGraphicsQueueFamilyIndex() const = 0;
    virtual uint32_t GetPresentQueueFamilyIndex() const = 0;
    virtual VkSurfaceKHR GetSurface() const = 0;
    virtual VkSwapchainKHR GetSwapchain() const = 0;
    virtual VkCommandBuffer GetCommandBuffer(uint32_t imageIndex) = 0;
    virtual VkFramebuffer GetFramebuffer(uint32_t imageIndex) = 0;
    virtual VkRenderPass GetRenderPass() const = 0;
    virtual VkPhysicalDeviceMemoryProperties
    GetPhysicalDeviceMemoryProperties() const = 0;
};

class IVulkanContextSetter {
public:
    virtual ~IVulkanContextSetter() = default;

    virtual void SetMemoryAllocator(CMemoryAllocator* allocator) = 0;
    virtual void SetDebugMessenger(VkDebugUtilsMessengerEXT debugMessenger) = 0;
    virtual void SetBufferHandler(CBufferHandler* bufferHandler) = 0;
    virtual void SetInstance(VkInstance instance) = 0;
    virtual void SetPhysicalDevice(VkPhysicalDevice physicalDevice) = 0;
    virtual void SetDevice(VkDevice device) = 0;
    virtual void SetGraphicsQueue(VkQueue graphicsQueue) = 0;
    virtual void SetPresentQueue(VkQueue presentQueue) = 0;
    virtual void
    SetGraphicsQueueFamilyIndex(uint32_t graphicsQueueFamilyIndex) = 0;
    virtual void
    SetPresentQueueFamilyIndex(uint32_t presentQueueFamilyIndex) = 0;
    virtual void SetSurface(VkSurfaceKHR surface) = 0;
    virtual void
    SetPhysicalDeviceProperties(VkPhysicalDeviceProperties properties) = 0;
    virtual void
    SetPhysicalDeviceFeatures(VkPhysicalDeviceFeatures features) = 0;
    virtual void SetPhysicalDeviceMemoryProperties(
        VkPhysicalDeviceMemoryProperties memoryProperties) = 0;

    virtual void SetSwapchain(VkSwapchainKHR swapchain) = 0;
    virtual void SetSwapchainImageFormat(VkFormat imageFormat) = 0;
    virtual void SetSwapchainExtent(VkExtent2D extent) = 0;
    virtual void SetSwapchainImages(std::vector<VkImage> images) = 0;
    virtual void
    SetSwapchainImageViews(std::vector<VkImageView> imageViews) = 0;
    virtual void SetRenderPass(VkRenderPass renderPass) = 0;
    virtual void SetCommandPool(VkCommandPool commandPool) = 0;
    virtual void
    SetCommandBuffers(std::vector<VkCommandBuffer> commandBuffers) = 0;
    virtual void SetFramebuffers(std::vector<VkFramebuffer> framebuffers) = 0;
};
} // namespace Vulkan