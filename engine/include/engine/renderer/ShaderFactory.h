#pragma once

#include <memory>
#include <string>

#include "engine/renderer/MaterialStructures.h"
#include <vulkan/vulkan.h>

namespace Vulkan {
class CVulkanContext;
} // namespace Vulkan

namespace Renderer {

class CShaderFactory {
public:
    CShaderFactory(const Vulkan::CVulkanContext& contextGetter);
    ~CShaderFactory();

    VkShaderModule CreateFragmentShader(std::string name, std::string path);
    VkShaderModule CreateVertexShader(std::string name, std::string path);

private:
    class CImpl;
    std::unique_ptr<CImpl> mImpl;
};
} // namespace Renderer