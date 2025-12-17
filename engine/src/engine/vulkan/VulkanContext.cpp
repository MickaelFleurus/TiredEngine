#include "engine/vulkan/VulkanContext.h"

#include "engine/vulkan/VulkanInitializer.h"

namespace Vulkan {

CVulkanContext::CVulkanContext(SDL_Window* window) : mWindow(window) {
}

CVulkanContext::~CVulkanContext() {
    vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
    for (auto framebuffer : mFramebuffers) {
        vkDestroyFramebuffer(mDevice, framebuffer, nullptr);
    }
    if (mRenderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
    }
    for (auto imageView : mImageViews) {
        vkDestroyImageView(mDevice, imageView, nullptr);
    }
    vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);
    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);

    if (mDebugMessenger != VK_NULL_HANDLE) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            mInstance, "vkDestroyDebugUtilsMessengerEXT");
        if (func)
            func(mInstance, mDebugMessenger, nullptr);
    }
    vkDestroyDevice(mDevice, nullptr);
    vkDestroyInstance(mInstance, nullptr);
}

void CVulkanContext::RecreateSwapchainResources() {
    vkDeviceWaitIdle(mDevice);

    for (auto framebuffer : mFramebuffers) {
        vkDestroyFramebuffer(mDevice, framebuffer, nullptr);
    }
    mFramebuffers.clear();

    if (mRenderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
    }

    for (auto imageView : mImageViews) {
        vkDestroyImageView(mDevice, imageView, nullptr);
    }
    mImageViews.clear();

    if (mSwapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);
    }

    auto [swapchain, imageFormat, extent, images, imageViews] =
        CreateSwapchain(mPhysicalDevice, mDevice, mSurface, mWindow,
                        mGraphicsQueueFamilyIndex, mPresentQueueFamilyIndex);

    mSwapchain = swapchain;
    mImageFormat = imageFormat;
    mExtent = extent;
    mImages = std::move(images);
    mImageViews = std::move(imageViews);

    mRenderPass = CreateRenderPass(mDevice, mImageFormat);

    mCommandBuffers = std::move(
        CreateCommandBuffers(mDevice, mCommandPool, mImageViews.size()));

    mFramebuffers = std::move(
        CreateFramebuffers(mDevice, mRenderPass, mImageViews, mExtent));
}

VkInstance CVulkanContext::GetInstance() const {
    return mInstance;
}

VkPhysicalDevice CVulkanContext::GetPhysicalDevice() const {
    return mPhysicalDevice;
}

VkDevice CVulkanContext::GetDevice() const {
    return mDevice;
}

VkQueue CVulkanContext::GetGraphicsQueue() const {
    return mGraphicsQueue;
}

VkQueue CVulkanContext::GetPresentQueue() const {
    return mPresentQueue;
}

uint32_t CVulkanContext::GetGraphicsQueueFamilyIndex() const {
    return mGraphicsQueueFamilyIndex;
}

uint32_t CVulkanContext::GetPresentQueueFamilyIndex() const {
    return mPresentQueueFamilyIndex;
}

VkSurfaceKHR CVulkanContext::GetSurface() const {
    return mSurface;
}

VkSwapchainKHR CVulkanContext::GetSwapchain() const {
    return mSwapchain;
}

VkCommandBuffer CVulkanContext::GetCommandBuffer(uint32_t imageIndex) {
    return mCommandBuffers[imageIndex];
}

VkFramebuffer CVulkanContext::GetFramebuffer(uint32_t imageIndex) {
    return mFramebuffers[imageIndex];
}

VkRenderPass CVulkanContext::GetRenderPass() const {
    return mRenderPass;
}

VkPhysicalDeviceMemoryProperties
CVulkanContext::GetPhysicalDeviceMemoryProperties() const {
    return mPhysicalDeviceMemoryProperties;
}

VkCommandPool CVulkanContext::GetCommandPool() const {
    return mCommandPool;
}

int CVulkanContext::GetImageCount() const {
    return static_cast<int>(mImages.size());
}

VkExtent2D CVulkanContext::GetSwapchainExtent() const {
    return mExtent;
}

VkPhysicalDeviceProperties CVulkanContext::GetPhysicalDeviceProperties() const {
    return mPhysicalDeviceProperties;
}

void CVulkanContext::SetSurface(VkSurfaceKHR surface) {
    mSurface = surface;
}

void CVulkanContext::SetInstance(VkInstance instance) {
    mInstance = instance;
}

void CVulkanContext::SetPhysicalDevice(VkPhysicalDevice physicalDevice) {
    mPhysicalDevice = physicalDevice;
}

void CVulkanContext::SetDevice(VkDevice device) {
    mDevice = device;
}

void CVulkanContext::SetGraphicsQueue(VkQueue graphicsQueue) {
    mGraphicsQueue = graphicsQueue;
}

void CVulkanContext::SetPresentQueue(VkQueue presentQueue) {
    mPresentQueue = presentQueue;
}

void CVulkanContext::SetGraphicsQueueFamilyIndex(
    uint32_t graphicsQueueFamilyIndex) {
    mGraphicsQueueFamilyIndex = graphicsQueueFamilyIndex;
}

void CVulkanContext::SetPresentQueueFamilyIndex(
    uint32_t presentQueueFamilyIndex) {
    mPresentQueueFamilyIndex = presentQueueFamilyIndex;
}

void CVulkanContext::SetDebugMessenger(
    VkDebugUtilsMessengerEXT debugMessenger) {
    mDebugMessenger = debugMessenger;
}

void CVulkanContext::SetPhysicalDeviceProperties(
    VkPhysicalDeviceProperties properties) {
    mPhysicalDeviceProperties = properties;
}

void CVulkanContext::SetPhysicalDeviceFeatures(
    VkPhysicalDeviceFeatures features) {
    mPhysicalDeviceFeatures = features;
}

void CVulkanContext::SetPhysicalDeviceMemoryProperties(
    VkPhysicalDeviceMemoryProperties memoryProperties) {
    mPhysicalDeviceMemoryProperties = memoryProperties;
}

void CVulkanContext::SetCommandPool(VkCommandPool commandPool) {
    mCommandPool = commandPool;
}

} // namespace Vulkan