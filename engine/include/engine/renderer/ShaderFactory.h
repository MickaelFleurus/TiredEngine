#pragma once

#include <memory>
#include <string>

#include <vulkan/vulkan.h>

namespace Renderer {
class VulkanRenderer;

struct SShaderResources {
    uint32_t numSamplers = 0;
    uint32_t numStorageTextures = 0;
    uint32_t numStorageBuffers = 0;
    uint32_t numUniformBuffers = 0;
};

class CShaderFactory {
public:
    CShaderFactory(const VulkanRenderer& renderer);
    ~CShaderFactory();

    VkShaderModule CreateFragmentShader(std::string name, std::string path,
                                        const SShaderResources& resources = {});
    VkShaderModule CreateVertexShader(std::string name, std::string path,
                                      const SShaderResources& resources = {});

private:
    class CImpl;
    std::unique_ptr<CImpl> mImpl;
};
} // namespace Renderer