#pragma once

#include "engine/renderer/MeshRenderer.h"
#include "engine/renderer/TextRenderer.h"

namespace Core {
class CCamera;
} // namespace Core

namespace Material {
class CMaterialManager;
} // namespace Material

namespace Vulkan {
class CBufferHandler;
} // namespace Vulkan

namespace Renderer {
class CRendererManager {
public:
    CRendererManager(Vulkan::CBufferHandler& bufferHandler,
                     Material::CMaterialManager& materialManager);
    ~CRendererManager();

    void FreeSceneData();
    void Prepare();

    CMeshRenderer& GetMeshRenderer();
    CTextRenderer& GetTextRenderer();
    void GenerateInstances(
        Renderer::CRenderables<Renderer::SMeshRenderable>& meshRenderables,
        Renderer::CRenderables<Renderer::STextRenderable>& textRenderables);
    void Render(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet,
                Core::CCamera& camera, Core::CCamera& uiCamera);

    static void NotifyGameObjectHidden(Core::GameObjectId id);

private:
    Vulkan::CBufferHandleWrapper<Core::SVertex>& mVertexBuffer;
    Vulkan::CBufferHandleWrapper<Core::IndexType>& mIndexesBuffer;
    Vulkan::CBufferHandleWrapper<Core::SInstanceData>& mInstanceBuffer;
    Vulkan::CBufferHandleWrapper<Core::SIndirectDrawCommand>&
        mIndirectDrawBuffer;
    Vulkan::CBufferHandleWrapper<Core::SUIInstanceData>& mTextInstanceBuffer;
    Vulkan::CBufferHandleWrapper<Core::SUIVertex>& mUIVertexBuffer;

    Material::CMaterialManager& mMaterialManager;
    CMeshRenderer mMeshRenderer;
    CTextRenderer mTextRenderer;

    static std::vector<Core::GameObjectId> mHiddenGameObjects;
};
} // namespace Renderer
