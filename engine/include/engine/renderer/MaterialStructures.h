#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace Renderer {

struct SPipelineDescriptors {
    VkPipeline pipeline = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
};
} // namespace Renderer