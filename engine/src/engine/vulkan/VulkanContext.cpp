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
                   uint32_t graphicsFamily, uint32_t presentFamily)
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
    , graphicsQueueFamilyIndex(graphicsFamily)
    , presentQueueFamilyIndex(presentFamily) {
}

SContext::~SContext() {
    vkDestroySurfaceKHR(instance, surface, nullptr);

    if (debugMessenger != VK_NULL_HANDLE) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func)
            func(instance, debugMessenger, nullptr);
    }
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
}

} // namespace Vulkan
