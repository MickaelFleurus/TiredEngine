#pragma once
#include <memory>
#include <vulkan/vulkan.h>

namespace Renderer {
struct SPipelineConfig;
class VulkanRenderer;

class CPipelineFactory {
public:
    CPipelineFactory(const VulkanRenderer& renderer);
    ~CPipelineFactory();

    VkPipeline CreateGraphicsPipeline(const SPipelineConfig& config);

private:
    class CImpl;
    std::unique_ptr<CImpl> mImpl;
};
} // namespace Renderer