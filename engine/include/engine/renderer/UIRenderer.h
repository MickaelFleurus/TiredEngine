#pragma once

#include "engine/core/DataTypes.h"
#include "engine/renderer/MaterialStructures.h"

namespace Vulkan {
class CHostBuffer;
class CPipelineFactory;
} // namespace Vulkan

namespace Component {
class CManager;
}

namespace Font {
class CFontHandler;
}

namespace System {
class CSystem;
}

namespace Renderer {

class CUiRenderer {
public:
    explicit CUiRenderer(Component::CManager& componentManager,
                         Font::CFontHandler& fontHandler,
                         System::CSystem& system,
                         Vulkan::CHostBuffer& instanceBuffer,
                         Vulkan::CPipelineFactory& pipelineFactory);

    void Prepare();
    void Update();
    void Render(VkCommandBuffer cmd, VkPipeline uiPipeline,
                Vulkan::CHostBuffer& instanceBuffer,
                VkPipelineLayout uiPipelineLayout);

private:
    Component::CManager& mComponentManager;
    Font::CFontHandler& mFontHandler;
    std::vector<Core::SScreenQuadInstance> mInstances;
    Core::PC::SUiPushConstants mPushConstants;
    Vulkan::CHostBuffer& mInstanceBuffer;
    Renderer::SPipelineDescriptors mUiPipelineDescriptors;
};

} // namespace Renderer
