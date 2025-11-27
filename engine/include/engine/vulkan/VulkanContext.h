#pragma once

#include <vector>
#include <vulkan/vulkan.h>

struct SDL_Window;

namespace Vulkan {
class CVulkanContext {
public:
    CVulkanContext(SDL_Window* window);
    ~CVulkanContext();

    VkInstance GetInstance() const;
    VkPhysicalDevice GetPhysicalDevice() const;
    VkDevice GetDevice() const;
    VkQueue GetGraphicsQueue() const;
    VkQueue GetPresentQueue() const;
    uint32_t GetGraphicsQueueFamilyIndex() const;
    uint32_t GetPresentQueueFamilyIndex() const;
    VkSurfaceKHR GetSurface() const;
    VkCommandBuffer GetCommandBuffer(uint32_t imageIndex);
    VkSwapchainKHR GetSwapchain() const;
    VkFramebuffer GetFramebuffer(uint32_t imageIndex);
    VkRenderPass GetRenderPass() const;
    VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties() const;
    VkCommandPool GetCommandPool() const;
    int GetImageCount() const;
    VkExtent2D GetSwapchainExtent() const;
    VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const;

    void RecreateSwapchainResources();

    void SetDebugMessenger(VkDebugUtilsMessengerEXT debugMessenger);

    void SetInstance(VkInstance instance);
    void SetPhysicalDevice(VkPhysicalDevice physicalDevice);
    void SetPhysicalDeviceProperties(VkPhysicalDeviceProperties properties);
    void SetPhysicalDeviceFeatures(VkPhysicalDeviceFeatures features);
    void SetPhysicalDeviceMemoryProperties(
        VkPhysicalDeviceMemoryProperties memoryProperties);
    void SetDevice(VkDevice device);
    void SetGraphicsQueue(VkQueue graphicsQueue);
    void SetPresentQueue(VkQueue presentQueue);
    void SetGraphicsQueueFamilyIndex(uint32_t graphicsQueueFamilyIndex);
    void SetPresentQueueFamilyIndex(uint32_t presentQueueFamilyIndex);
    void SetSurface(VkSurfaceKHR surface);

    void SetCommandPool(VkCommandPool commandPool);

private:
    SDL_Window* mWindow;

    VkInstance mInstance;
    VkSurfaceKHR mSurface;
    VkDebugUtilsMessengerEXT mDebugMessenger;

    // Logical device
    VkDevice mDevice = VK_NULL_HANDLE;
    VkQueue mGraphicsQueue = VK_NULL_HANDLE;
    VkQueue mPresentQueue = VK_NULL_HANDLE;
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
    VkRenderPass mRenderPass = VK_NULL_HANDLE;
    int mImagesCount = 0;
    VkCommandPool mCommandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> mCommandBuffers;
    std::vector<VkFramebuffer> mFramebuffers;
};
} // namespace Vulkan