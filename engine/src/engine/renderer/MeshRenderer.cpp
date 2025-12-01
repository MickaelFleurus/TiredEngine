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
    mMeshDrawRanges.clear();
    mMeshInstanceRanges.clear();
}

void CMeshRenderer::Prepare() {
    for (const auto* mesh : mRegisteredMeshes) {
        if (mMeshDrawRanges.contains(mesh->GetHash())) {
            continue;
        }

        auto verticesRange = mVertexBufferHandle.Prepare(mesh->GetVertices());
        auto indexesRange = mIndexesBufferHandle.Prepare(mesh->GetIndexes());

        if (!verticesRange.has_value() || !indexesRange.has_value()) {
            LOG_ERROR("Failed to register mesh!");
            continue;
        }

        mMeshDrawRanges.emplace(
            mesh->GetHash(),
            SMeshDrawData{.verticesRange = verticesRange.value(),
                          .indexesRange = indexesRange.value()});
    }
    mRegisteredMeshes.clear();
}

void CMeshRenderer::Update() {
    // Implementation for updating mesh data goes here
}

void CMeshRenderer::RegisterMesh(const Core::CMesh* mesh) {
    mRegisteredMeshes.push_back(mesh);
}

std::unordered_map<Material::CAbstractMaterial*,
                   std::vector<Utils::SBufferIndexRange>>
CMeshRenderer::RebuildInstances(const std::vector<SRenderable>& renderables) {

    // Vector of pair <meshHash, instance index>
    std::unordered_map<
        Material::CAbstractMaterial*,
        std::unordered_map<std::size_t, std::vector<Core::SInstanceData>>>
        renderablesByMaterial;

    for (auto& renderable : renderables) {
        Core::SInstanceData instanceData{};
        instanceData.modelMatrix = renderable.transform;
        instanceData.color = renderable.color;
        instanceData.materialId = 0; // FIXME: Assign proper material ID
        instanceData.textureId = renderable.textureIndex;

        renderablesByMaterial[renderable.material][renderable.meshHash]
            .push_back(instanceData);
    }
    std::unordered_map<Material::CAbstractMaterial*,
                       std::vector<Utils::SBufferIndexRange>>
        drawCommands;

    for (const auto& [material, meshesInstances] : renderablesByMaterial) {
        for (const auto& [meshHash, instances] : meshesInstances) {
            auto drawDataIt = mMeshDrawRanges.find(meshHash);
            if (drawDataIt == mMeshDrawRanges.end()) {
                LOG_WARNING("Mesh not registered for rendering!");
                continue;
            }
            const auto& drawData = drawDataIt->second;

            auto instanceRange = mInstancesBuffer.Prepare(instances);
            if (!instanceRange.has_value()) {
                LOG_ERROR("Failed to prepare instance data!");
                continue;
            }
            Core::SIndirectDrawCommand drawCommand{};
            drawCommand.indexCount =
                mMeshDrawRanges[meshHash].indexesRange.count;
            drawCommand.instanceCount =
                static_cast<uint32_t>(instanceRange->count);
            drawCommand.firstIndex = static_cast<uint32_t>(
                mMeshDrawRanges[meshHash].indexesRange.first);
            drawCommand.vertexOffset =
                static_cast<int>(mMeshDrawRanges[meshHash].verticesRange.first);
            drawCommand.firstInstance =
                static_cast<uint32_t>(instanceRange->first);
            auto indirectRange = mIndirectDrawBuffer.Prepare({drawCommand});
            if (indirectRange.has_value()) {
                drawCommands[material].push_back(indirectRange.value());
            } else {
                LOG_ERROR("Failed to prepare indirect draw command!");
                continue;
            }
        }
    }
    return std::move(drawCommands);
}

void CMeshRenderer::Draw(VkCommandBuffer commandBuffer) {
    for (const auto& [meshHash, drawData] : mMeshDrawRanges) {
        vkCmdDrawIndexed(
            commandBuffer, static_cast<uint32_t>(drawData.indexesRange.count),
            1, drawData.indexesRange.first, drawData.verticesRange.first, 0);
    }
}
} // namespace Renderer