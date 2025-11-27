#pragma once
#include <memory>
#include <vulkan/vulkan.h>

#include "engine/renderer/MaterialStructures.h"

namespace Vulkan {
class CVulkanContext;
}

namespace Renderer {
struct SPipelineConfig;
class CDescriptorStorage;

class CPipelineFactory {
public:
    CPipelineFactory(const Vulkan::CVulkanContext& contextGetter);
    ~CPipelineFactory();

    SPipelineDescriptors
    CreateGraphicsPipeline(const SPipelineConfig& config,
                           CDescriptorStorage& layoutStorage);

private:
    class CImpl;
    std::unique_ptr<CImpl> mImpl;
};
} // namespace Renderer