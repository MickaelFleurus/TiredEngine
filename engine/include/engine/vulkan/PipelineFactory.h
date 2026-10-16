#pragma once
#include <memory>

#include "engine/renderer/MaterialStructures.h"

namespace Renderer {
struct SPipelineConfig;
struct SComputePipelineConfig;
} // namespace Renderer

namespace Vulkan {
struct SContext;
class CSwapchain;

class CPipelineFactory {
public:
    CPipelineFactory(const Vulkan::SContext& context,
                     Vulkan::CSwapchain& swapchain);
    ~CPipelineFactory();

    Renderer::SPipelineDescriptors
    GetOrCreateGraphicsPipeline(const Renderer::SPipelineConfig& config);
    Renderer::SPipelineDescriptors
    GetOrCreateComputePipeline(const Renderer::SComputePipelineConfig& config);

private:
    class CImpl;
    std::unique_ptr<CImpl> mImpl;
};
} // namespace Vulkan
