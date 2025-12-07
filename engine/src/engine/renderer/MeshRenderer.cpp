#include "engine/renderer/MeshRenderer.h"

#include "engine/core/Mesh.h"
#include "engine/utils/Logger.h"
#include "engine/vulkan/BufferHandleWrapper.h"

namespace Renderer {
CMeshRenderer::CMeshRenderer(
    Vulkan::CBufferHandleWrapper<Core::SVertex>& vertexBufferHandle,
    Vulkan::CBufferHandleWrapper<Core::IndexType>& indexesBufferHandle,
    Vulkan::CBufferHandleWrapper<Core::SInstanceData>& instancesBuffer,
    Vulkan::CBufferHandleWrapper<Core::SIndirectDrawCommand>&
        indirectDrawBuffer)
    : mVertexBufferHandle(vertexBufferHandle)
    , mIndexesBufferHandle(indexesBufferHandle)
    , mInstancesBuffer(instancesBuffer)
    , mIndirectDrawBuffer(indirectDrawBuffer) {
}

CMeshRenderer::~CMeshRenderer() = default;

void CMeshRenderer::Free() {
    // mMeshDrawRanges.clear();
    // mMeshInstancesRanges.clear();
}

void CMeshRenderer::Update() {
    // Implementation for updating mesh data goes here
}

void CMeshRenderer::RegisterMesh(const Core::CMesh* mesh) {
    if (mMeshDrawRanges.contains(mesh->GetHash())) {
        return;
    }

    Utils::SBufferIndexRange verticesRange{};
    Utils::SBufferIndexRange indexesRange{};
    bool dataPrepared =
        mVertexBufferHandle.PrepareData(verticesRange, mesh->GetVertices());
    dataPrepared = dataPrepared && mIndexesBufferHandle.PrepareData(
                                       indexesRange, mesh->GetIndexes());

    if (!dataPrepared) {
        LOG_FATAL(
            "Failed to register mesh! This is the symptom of too many unique "
            "meshes being created or a buffer being way too small.");
        return;
    }

    mMeshDrawRanges.emplace(mesh->GetHash(),
                            SMeshDrawData{.verticesRange = verticesRange,
                                          .indexesRange = indexesRange});
}

void CMeshRenderer::UpdateInstances(
    const std::vector<SRenderable>& renderables) {
    std::unordered_map<std::pair<std::size_t, std::size_t>, SMeshPipelineGroup,
                       SPairHash>
        instancesAddedOrModified;
    for (const auto& renderable : renderables) {
        auto key =
            std::make_pair(renderable.meshHash, renderable.material->GetId());
        instancesAddedOrModified[key].instancesData.push_back(
            Core::SInstanceData{
                .modelMatrix = renderable.transform,
                .color = renderable.color,
                .materialId = static_cast<int>(renderable.material->GetId()),
                .textureId = renderable.textureIndex,
            });
    }

    for (auto& [key, pipelineGroup] : instancesAddedOrModified) {
        auto& alreadyUploaded = mInstanceCache[key];
        const auto uploadedSize = alreadyUploaded.instancesData.size();
        const auto newSize = pipelineGroup.instancesData.size();

        if (uploadedSize == newSize) {
            alreadyUploaded.instancesData.swap(pipelineGroup.instancesData);
        } else {
            alreadyUploaded.instanceBufferRange.count = newSize;
            alreadyUploaded.instancesData.swap(pipelineGroup.instancesData);
        }

        if (!mInstancesBuffer.PrepareData(alreadyUploaded.instanceBufferRange,
                                          alreadyUploaded.instancesData)) {
            LOG_WARNING("Failed to prepare instance data for mesh renderer!");
        }

        Core::SIndirectDrawCommand indirectCommand{};
        indirectCommand.indexCount = static_cast<uint32_t>(
            mMeshDrawRanges[key.first].indexesRange.count);
        indirectCommand.instanceCount = newSize;
        indirectCommand.firstIndex = static_cast<uint32_t>(
            mMeshDrawRanges[key.first].indexesRange.first);
        indirectCommand.vertexOffset = static_cast<int32_t>(
            mMeshDrawRanges[key.first].verticesRange.first);
        indirectCommand.firstInstance =
            static_cast<uint32_t>(alreadyUploaded.instanceBufferRange.first);
        if (!mIndirectDrawBuffer.PrepareData(
                alreadyUploaded.indirectBufferRange, indirectCommand)) {
            LOG_WARNING("Failed to prepare indirect draw command for mesh "
                        "renderer!");
        }
    }
}

} // namespace Renderer