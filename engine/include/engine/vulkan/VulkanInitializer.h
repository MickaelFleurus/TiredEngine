#pragma once

#include <tuple>
#include <vector>
#include <vulkan/vulkan.h>
struct SDL_Window;

namespace System {
class CSystem;
} // namespace System

namespace Vulkan {
class CVulkanContext;
void InitializeVulkan(CVulkanContext& context, SDL_Window* window,
                      const System::CSystem& system);
std::tuple<VkSwapchainKHR, VkFormat, VkExtent2D, std::vector<VkImage>,
           std::vector<VkImageView>>
CreateSwapchain(VkPhysicalDevice physicalDevice, VkDevice device,
                VkSurfaceKHR surface, SDL_Window* window,
                uint32_t graphicsFamily, uint32_t presentFamily);
VkRenderPass CreateRenderPass(VkDevice device, VkFormat swapchainFormat);
std::vector<VkCommandBuffer> CreateCommandBuffers(VkDevice device,
                                                  VkCommandPool& commandPool,
                                                  int bufferCount);
std::vector<VkFramebuffer>
CreateFramebuffers(VkDevice device, VkRenderPass renderPass,
                   const std::vector<VkImageView>& imageViews,
                   VkExtent2D extent);

} // namespace Vulkan