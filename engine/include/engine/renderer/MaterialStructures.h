#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace Renderer {

struct SShaderDescriptors {
    VkShaderModule shaderModule = VK_NULL_HANDLE;
    std::vector<size_t> descriptorSetLayoutsHashes;
};

struct SPipelineDescriptors {
    VkPipeline pipeline = VK_NULL_HANDLE;
    std::vector<size_t> descriptorSetLayoutsHashes;
};
} // namespace Renderer