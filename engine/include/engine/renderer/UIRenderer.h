#pragma once

#include "engine/core/DataTypes.h"
#include "engine/renderer/MaterialStructures.h"

namespace Core {
class CEntityManager;
}

namespace Vulkan {
class CHostBuffer;
class CPipelineFactory;
struct SContext;
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

namespace Utils {
class CFileHandler;
}

namespace Renderer {

class CUiRenderer {
public:
    explicit CUiRenderer(const Vulkan::SContext& context,
                         Component::CManager& componentManager,
                         Font::CFontHandler& fontHandler,
                         System::CSystem& system,
                         Vulkan::CHostBuffer& instanceBuffer,
                         Vulkan::CPipelineFactory& pipelineFactory,
                         Core::CEntityManager& entityManager);

    void Prepare();
    void Update();
    void Render(VkCommandBuffer cmd);

private:
    Component::CManager& mComponentManager;
    Font::CFontHandler& mFontHandler;
    std::vector<Core::SScreenQuadInstance> mInstances;
    Core::PC::SUiPushConstants mPushConstants;
    Vulkan::CHostBuffer& mInstanceBuffer;
    Renderer::SPipelineDescriptors mUiPipelineDescriptors;
    const Vulkan::SContext& mContext;
    Core::CEntityManager& mEntityManager;
};

} // namespace Renderer
