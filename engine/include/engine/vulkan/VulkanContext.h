#pragma once

#include <memory>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
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
             VkQueue graphicsQueue, VkQueue presentQueue, VkQueue transferQueue,
             uint32_t graphicsFamily, uint32_t presentFamily,
             uint32_t transferFamily, VmaAllocator vmaAllocator,
             VkDescriptorPool descriptorPool,
             VkDescriptorSetLayout bindelessTextureSetLayout,
             VkDescriptorSet bindelessTextureSet);
    ~SContext();

    const std::unique_ptr<SDL_Window, void (*)(SDL_Window*)> window;

    const VkInstance instance;
    const VkSurfaceKHR surface;
    const VkDebugUtilsMessengerEXT debugMessenger;

    // Logical device
    const VkDevice device;
    const VkQueue graphicsQueue;
    const VkQueue presentQueue;
    const VkQueue transferQueue;
    const uint32_t graphicsQueueFamilyIndex;
    const uint32_t presentQueueFamilyIndex;
    const uint32_t transferQueueFamilyIndex;

    // Physical device
    const VkPhysicalDevice physicalDevice;
    const VkPhysicalDeviceProperties physicalDeviceProperties;
    const VkPhysicalDeviceFeatures physicalDeviceFeatures;
    const VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

    // VMA Allocator
    const VmaAllocator vmaAllocator;

    // Bindless Texture pool
    const VkDescriptorPool descriptorPool;
    const VkDescriptorSetLayout bindelessTextureSetLayout;
    const VkDescriptorSet bindelessTextureSet;
};
} // namespace Vulkan
