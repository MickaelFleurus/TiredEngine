#pragma once
#include "engine/core/DataTypes.h"
#include "engine/renderer/IRenderer.h"
#include "engine/renderer/RendererUtils.h"
#include "engine/utils/BufferMemoryBlocks.h"

#include <span>
#include <unordered_map>
#include <vector>

namespace Core {
class CMesh;
class CCamera;
} // namespace Core

namespace Vulkan {
template <typename T>
class CBufferHandleWrapper;
} // namespace Vulkan

namespace Renderer {

class CMeshRenderer : public IRenderer {
public:
    CMeshRenderer(
        Vulkan::CBufferHandleWrapper<Core::SVertex>& vertexBufferHandle,
        Vulkan::CBufferHandleWrapper<Core::IndexType>& indexesBufferHandle,
        Vulkan::CBufferHandleWrapper<Core::SInstanceData>& instancesBuffer,
        Vulkan::CBufferHandleWrapper<Core::SIndirectDrawCommand>&
            indirectDrawBuffer);
    ~CMeshRenderer();

    void RegisterMesh(const Core::CMesh* mesh);

    void Free() override;
    void Prepare() override;
    void Update() override;

    std::unordered_map<Material::CAbstractMaterial*,
                       std::vector<Utils::SBufferIndexRange>>
    RebuildInstances(const std::vector<SRenderable>& renderables) override;

    void Draw(VkCommandBuffer commandBuffer);

private:
    struct SMeshDrawData {
        Utils::SBufferIndexRange verticesRange{};
        Utils::SBufferIndexRange indexesRange{};
    };
    struct SMeshInstanceData {
        int indexOffset{0};
        int vertexOffset{0};
        int indexCount{0};
        int instanceCount{0};
        int instanceOffset{0};
    };

    Vulkan::CBufferHandleWrapper<Core::SVertex>& mVertexBufferHandle;
    Vulkan::CBufferHandleWrapper<Core::IndexType>& mIndexesBufferHandle;
    Vulkan::CBufferHandleWrapper<Core::SInstanceData>& mInstancesBuffer;
    Vulkan::CBufferHandleWrapper<Core::SIndirectDrawCommand>&
        mIndirectDrawBuffer;

    std::vector<const Core::CMesh*> mRegisteredMeshes;
    std::unordered_map<std::size_t, SMeshDrawData> mMeshDrawRanges;
    std::unordered_map<std::size_t, SMeshInstanceData> mMeshInstanceRanges;
};
} // namespace Renderer