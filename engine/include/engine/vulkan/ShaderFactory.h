#pragma once

#include <memory>
#include <string>

#include <vulkan/vulkan.h>

namespace Vulkan {
struct SContext;

class CShaderFactory {
public:
    CShaderFactory(const SContext& context);
    ~CShaderFactory();

    VkShaderModule CreateFragmentShader(std::string name, std::string path);
    VkShaderModule CreateVertexShader(std::string name, std::string path);
    VkShaderModule CreateComputeShader(std::string name, std::string path);

private:
    class CImpl;
    std::unique_ptr<CImpl> mImpl;
};
} // namespace Vulkan
