#pragma once

#include "engine/renderer/MaterialStructures.h"
#include "engine/renderer/MeshRenderer.h"

namespace Core {
class CCamera;
} // namespace Core

namespace Material {
class CMaterialManager;
} // namespace Material

namespace Vulkan {
class CBufferHandler;
class CPipelineFactory;
} // namespace Vulkan

namespace Component {
class CComponentManager;
}

namespace Renderer {
class CTransformManager;
struct SPipelineDescriptors;

class CRendererManager {
public:
    CRendererManager(Vulkan::CBufferHandler& bufferHandler,
                     Material::CMaterialManager& materialManager,
                     CTransformManager& transformManager,
                     Vulkan::CPipelineFactory& pipelineFactory);
    ~CRendererManager();

    void FreeSceneData();
    void Prepare();

    CMeshRenderer& GetMeshRenderer();
    void GenerateInstances(Component::CComponentManager& componentManager);
    void Render(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet,
                Core::CCamera& camera, Core::CCamera& uiCamera);

    static void NotifyGameObjectHidden(Core::GameObjectId id);

private:
    Vulkan::CBufferHandler& mBufferHandler;
    Vulkan::CPipelineFactory& mPipelineFactory;

    Material::CMaterialManager& mMaterialManager;
    CTransformManager& mTransformManager;
    CMeshRenderer mMeshRenderer;

    Renderer::SPipelineDescriptors mCullPipeline;
};
} // namespace Renderer
