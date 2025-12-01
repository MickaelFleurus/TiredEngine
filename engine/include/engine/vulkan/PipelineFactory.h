#pragma once
#include <memory>

#include "engine/renderer/MaterialStructures.h"

namespace Renderer {
struct SPipelineConfig;
} // namespace Renderer

namespace Vulkan {
class CVulkanContext;
class CDescriptorStorage;

class CPipelineFactory {
public:
    CPipelineFactory(const Vulkan::CVulkanContext& contextGetter);
    ~CPipelineFactory();

    Renderer::SPipelineDescriptors
    GetOrCreateGraphicsPipeline(const Renderer::SPipelineConfig& config,
                                Vulkan::CDescriptorStorage& layoutStorage);

private:
    class CImpl;
    std::unique_ptr<CImpl> mImpl;
};
} // namespace Vulkan