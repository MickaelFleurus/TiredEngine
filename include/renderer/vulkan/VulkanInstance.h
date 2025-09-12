#pragma once

#include <vulkan/vulkan.h>

namespace Renderer {
class CVulkanInstance {
public:
    CVulkanInstance();
    ~CVulkanInstance();

    bool CreateInstance();
    bool CreateValidationLayers();
    bool CreateDebugMessenger();

private:
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    VkInstance m_Instance;
};
} // namespace Renderer