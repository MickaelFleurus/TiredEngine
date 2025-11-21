#pragma once
#include <memory>
#include <vulkan/vulkan.h>

#include "engine/renderer/MaterialStructures.h"

namespace Renderer {
struct SPipelineConfig;
class CDescriptorLayoutStorage;
class CWindow;

class CPipelineFactory {
public:
    CPipelineFactory(const CWindow& window,
                     CDescriptorLayoutStorage& descriptorLayoutStorage);
    ~CPipelineFactory();

    SPipelineDescriptors CreateGraphicsPipeline(const SPipelineConfig& config);

private:
    class CImpl;
    std::unique_ptr<CImpl> mImpl;
};
} // namespace Renderer