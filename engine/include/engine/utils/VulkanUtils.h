#pragma once

#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

struct SDL_Window;

namespace Utils {

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct VulkanDevice {
    VkDevice device = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
    uint32_t graphicsFamily = 0;
    uint32_t presentFamily = 0;
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct VulkanSwapchain {
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkFormat imageFormat;
    VkExtent2D extent;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
};

VulkanSwapchain CreateSwapchain(VkPhysicalDevice physicalDevice,
                                VkDevice device, VkSurfaceKHR surface,
                                SDL_Window* window, uint32_t graphicsFamily,
                                uint32_t presentFamily);
VkRenderPass CreateRenderPass(VkDevice device, VkFormat imageFormat);
VkSurfaceFormatKHR
ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& modes);
VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& caps,
                        SDL_Window* window);
SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device,
                                              VkSurfaceKHR surface);
VulkanDevice CreateLogicalDevice(VkPhysicalDevice physicalDevice,
                                 VkSurfaceKHR surface);
VkSurfaceKHR CreateVulkanSurface(SDL_Window* window, VkInstance instance);
QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device,
                                     VkSurfaceKHR surface);
VkPhysicalDevice PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
void CreateVulkanCallback(VkInstance instance);
} // namespace Utils