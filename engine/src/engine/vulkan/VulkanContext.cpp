#include "engine/vulkan/VulkanContext.h"

namespace Vulkan {

SContext::SContext(std::unique_ptr<SDL_Window, void (*)(SDL_Window*)>&& window,
                   VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                   VkSurfaceKHR surface, VkDevice device,
                   VkPhysicalDevice physicalDevice,
                   VkPhysicalDeviceProperties properties,
                   VkPhysicalDeviceFeatures features,
                   VkPhysicalDeviceMemoryProperties memoryProperties,
                   VkQueue graphicsQueue, VkQueue presentQueue,
                   VkQueue transferQueue, uint32_t graphicsFamily,
                   uint32_t presentFamily, uint32_t transferFamily,
                   VmaAllocator vmaAllocator, VkDescriptorPool descriptorPool,
                   VkDescriptorSetLayout bindelessTextureSetLayout,
                   VkDescriptorSet bindelessTextureSet)
    : window(std::move(window))
    , instance(instance)
    , debugMessenger(debugMessenger)
    , surface(surface)
    , device(device)
    , physicalDevice(physicalDevice)
    , physicalDeviceProperties(properties)
    , physicalDeviceFeatures(features)
    , physicalDeviceMemoryProperties(memoryProperties)
    , graphicsQueue(graphicsQueue)
    , presentQueue(presentQueue)
    , transferQueue(transferQueue)
    , graphicsQueueFamilyIndex(graphicsFamily)
    , presentQueueFamilyIndex(presentFamily)
    , transferQueueFamilyIndex(transferFamily)
    , vmaAllocator(vmaAllocator)
    , descriptorPool(descriptorPool)
    , bindelessTextureSetLayout(bindelessTextureSetLayout)
    , bindelessTextureSet(bindelessTextureSet) {
}

SContext::~SContext() {
    vkDestroyDescriptorSetLayout(device, bindelessTextureSetLayout, nullptr);
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);

    if (debugMessenger != VK_NULL_HANDLE) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func)
            func(instance, debugMessenger, nullptr);
    }
    vkDestroyDevice(device, nullptr);
    vmaDestroyAllocator(vmaAllocator);
    vkDestroyInstance(instance, nullptr);
}

} // namespace Vulkan
