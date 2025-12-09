#pragma once

#include "engine/renderer/MeshRenderer.h"
#include "engine/renderer/TextRenderer.h"

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

    CMeshRenderer& GetMeshRenderer();
    CTextRenderer& GetTextRenderer();
    void GenerateInstances(const std::vector<SRenderable>& renderables);
    void Render(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet,
                Core::CCamera& camera);

private:
    Vulkan::CBufferHandleWrapper<Core::SVertex>& mVertexBuffer;
    Vulkan::CBufferHandleWrapper<Core::IndexType>& mIndexesBuffer;
    Vulkan::CBufferHandleWrapper<Core::SInstanceData>& mInstanceBuffer;
    Vulkan::CBufferHandleWrapper<Core::SIndirectDrawCommand>&
        mIndirectDrawBuffer;
    Vulkan::CBufferHandleWrapper<Core::STextInstanceData>& mTextInstanceBuffer;

    Material::CMaterialManager& mMaterialManager;
    CMeshRenderer mMeshRenderer;
    CTextRenderer mTextRenderer;
};
} // namespace Renderer