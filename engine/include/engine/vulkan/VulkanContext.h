#pragma once

#include <memory>

#include <vulkan/vulkan.h>

struct SDL_Window;

namespace Vulkan {
struct SContext {
public:
    SContext(std::unique_ptr<SDL_Window, void (*)(SDL_Window*)>&& window,
             VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
             VkSurfaceKHR surface, VkDevice device,
             VkPhysicalDevice physicalDevice,
             VkPhysicalDeviceProperties properties,
             VkPhysicalDeviceFeatures features,
             VkPhysicalDeviceMemoryProperties memoryProperties,
             VkQueue graphicsQueue, VkQueue presentQueue,
             uint32_t graphicsFamily, uint32_t presentFamily);
    ~SContext();

    const std::unique_ptr<SDL_Window, void (*)(SDL_Window*)> window;

    const VkInstance instance;
    const VkSurfaceKHR surface;
    const VkDebugUtilsMessengerEXT debugMessenger;

    // Logical device
    const VkDevice device;
    const VkQueue graphicsQueue;
    const VkQueue presentQueue;
    const uint32_t graphicsQueueFamilyIndex;
    const uint32_t presentQueueFamilyIndex;

    // Physical device
    const VkPhysicalDevice physicalDevice;
    const VkPhysicalDeviceProperties physicalDeviceProperties;
    const VkPhysicalDeviceFeatures physicalDeviceFeatures;
    const VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
};
} // namespace Vulkan
