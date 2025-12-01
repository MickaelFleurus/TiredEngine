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
    void UploadSceneData();

    CMeshRenderer& GetMeshRenderer();
    CTextRenderer& GetTextRenderer();
    void RebuildInstances(const std::vector<SRenderable>& renderables);

    void Render(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet,
                Core::CCamera& camera);

private:
    Vulkan::CBufferHandler& mBufferHandler;
    Material::CMaterialManager& mMaterialManager;
    CMeshRenderer mMeshRenderer;
    CTextRenderer mTextRenderer;
    std::unordered_map<Material::CAbstractMaterial*,
                       std::vector<Utils::SBufferIndexRange>>
        mMeshInstanceRanges;
};
} // namespace Renderer