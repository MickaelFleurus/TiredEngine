#include "engine/vulkan/VulkanContext.h"

namespace Vulkan {

CVulkanContext::~CVulkanContext() {
    vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
    for (auto framebuffer : mFramebuffers) {
        vkDestroyFramebuffer(mDevice, framebuffer, nullptr);
    }
    vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
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

void CVulkanContext::SetSwapchain(VkSwapchainKHR swapchain) {
    mSwapchain = swapchain;
}

void CVulkanContext::SetSwapchainImageFormat(VkFormat imageFormat) {
    mImageFormat = imageFormat;
}

void CVulkanContext::SetSwapchainExtent(VkExtent2D extent) {
    mExtent = extent;
}

void CVulkanContext::SetSwapchainImages(const std::vector<VkImage>& images) {
    mImages = images;
}

void CVulkanContext::SetSwapchainImageViews(
    std::vector<VkImageView> imageViews) {
    mImageViews = std::move(imageViews);
}

void CVulkanContext::SetRenderPass(VkRenderPass renderPass) {
    mRenderPass = renderPass;
}

void CVulkanContext::SetCommandPool(VkCommandPool commandPool) {
    mCommandPool = commandPool;
}

void CVulkanContext::SetCommandBuffers(
    std::vector<VkCommandBuffer> commandBuffers) {
    mCommandBuffers = std::move(commandBuffers);
}

void CVulkanContext::SetFramebuffers(std::vector<VkFramebuffer> framebuffers) {
    mFramebuffers = std::move(framebuffers);
}

} // namespace Vulkan