#pragma once
#include <memory>

#include "engine/renderer/MaterialStructures.h"

namespace Renderer {
struct SPipelineConfig;
} // namespace Renderer

namespace Vulkan {
struct SContext;
class CSwapchain;
class CDescriptorStorage;

class CPipelineFactory {
public:
    CPipelineFactory(const Vulkan::SContext& context,
                     Vulkan::CSwapchain& swapchain);
    ~CPipelineFactory();

    Renderer::SPipelineDescriptors
    GetOrCreateGraphicsPipeline(const Renderer::SPipelineConfig& config,
                                Vulkan::CDescriptorStorage& layoutStorage);

private:
    class CImpl;
    std::unique_ptr<CImpl> mImpl;
};
} // namespace Vulkan
