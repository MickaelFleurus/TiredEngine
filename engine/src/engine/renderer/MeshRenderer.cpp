#include "engine/renderer/MeshRenderer.h"

#include <set>

#include "engine/component/MeshComponent.h"
#include "engine/core/Mesh.h"
#include "engine/material/AbstractMaterial.h"
#include "engine/renderer/TransformManager.h"
#include "engine/utils/Logger.h"
#include "engine/vulkan/BufferHandleWrapper.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace Renderer {
CMeshRenderer::CMeshRenderer(
    Vulkan::CBufferHandleWrapper<Core::SVertex>& vertexBufferHandle,
    Vulkan::CBufferHandleWrapper<Core::IndexType>& indexesBufferHandle,
    Vulkan::CBufferHandleWrapper<Core::SInstanceData>& instancesBuffer,
    Vulkan::CBufferHandleWrapper<Core::SIndirectDrawCommand>&
        indirectDrawBuffer,
    CTransformManager& transformManager)
    : mVertexBufferHandle(vertexBufferHandle)
    , mIndexesBufferHandle(indexesBufferHandle)
    , mInstancesBuffer(instancesBuffer)
    , mIndirectDrawBuffer(indirectDrawBuffer)
    , mTransformManager(transformManager) {
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

void CMeshRenderer::UnregisterMesh(const Core::CMesh* mesh) {
    auto it = mMeshDrawRanges.find(mesh->GetHash());
    if (it != mMeshDrawRanges.end()) {
        const auto& drawData = it->second;
        mVertexBufferHandle.FreeRange(drawData.verticesRange);
        mIndexesBufferHandle.FreeRange(drawData.indexesRange);
        mMeshDrawRanges.erase(it);
    }
}

void CMeshRenderer::UpdateInstances(
    std::vector<std::unique_ptr<Component::IComponent>>& meshComponents,
    const std::vector<Core::GameObjectId>& hidden) {

    std::set<std::pair<std::size_t, std::size_t>> requireInstanceUpdate;
    std::set<std::pair<std::size_t, std::size_t>> requireIndirectUpdate;

    for (std::size_t i = 0; i < meshComponents.size(); ++i) {
        auto* meshComponent =
            static_cast<Component::CMeshComponent*>(meshComponents[i].get());
        if (!meshComponent->IsDirty()) {
            continue;
        }
        const auto* mesh = meshComponent->GetMesh();
        const auto dirtyFlag = meshComponent->GetDirtyFlag();
        const auto materialId = mesh->GetMaterial()->GetId();
        const auto key = std::make_pair(mesh->GetHash(), materialId);
        const auto goId = meshComponent->GetId();
        const auto& modelMatrix = mTransformManager.GetWorld(goId);
        if (Core::RequireReordering(dirtyFlag)) {

            RegisterMesh(meshComponent->GetMesh());
            requireInstanceUpdate.emplace(key);
            requireIndirectUpdate.emplace(key);

            auto& group = mInstanceCache[key];
            Core::SInstanceData instanceData{};
            instanceData.modelMatrix = modelMatrix;
            instanceData.color = meshComponent->GetColor();
            instanceData.materialId = materialId;
            instanceData.textureId = meshComponent->GetTextureIndex();
            if (auto instanceIndex = mInstanceCache[key].GetInstanceIndex(goId);
                instanceIndex.has_value()) {
                group.instancesData[*instanceIndex] = instanceData;

            } else {
                group.instancesData.push_back(instanceData);
                group.gameObjectIds.push_back(goId);
            }
        } else {
            requireInstanceUpdate.emplace(key);
            auto instanceIndex = mInstanceCache[key].GetInstanceIndex(goId);
            if (instanceIndex.has_value()) {
                auto& instanceData =
                    mInstanceCache[key].instancesData[*instanceIndex];
                instanceData.modelMatrix = modelMatrix;
                instanceData.color = meshComponent->GetColor();
                instanceData.textureId = meshComponent->GetTextureIndex();
            } else {
                LOG_WARNING(
                    "Renderable with id {} not found in instance cache.",
                    goId.index);
            }
        }
        meshComponent->Clean();
    }
    for (const auto& id : hidden) {
        for (auto& [key, group] : mInstanceCache) {
            auto instanceIndex = group.GetInstanceIndex(id);
            if (instanceIndex.has_value()) {
                requireIndirectUpdate.emplace(key);
                group.instancesData.erase(group.instancesData.begin() +
                                          *instanceIndex);
                group.gameObjectIds.erase(group.gameObjectIds.begin() +
                                          *instanceIndex);
            }
        }
    }

    // for (const auto& renderable : renderables.GetUpdateRenderables()) {
    //     auto key = std::make_pair(renderable.meshHash,
    //     renderable.materialId); requireInstanceUpdate.emplace(key); auto
    //     instanceIndex =
    //         mInstanceCache[key].GetInstanceIndex(renderable.id);
    //     if (instanceIndex.has_value()) {
    //         auto& instanceData =
    //             mInstanceCache[key].instancesData[*instanceIndex];
    //         instanceData.modelMatrix = renderable.transform;
    //         instanceData.color = renderable.color;
    //         instanceData.materialId = renderable.materialId;
    //         instanceData.textureId = renderable.textureIndex;
    //     } else {
    //         LOG_WARNING("Renderable with id {} not found in instance cache.",
    //                     renderable.id.index);
    //     }
    // }

    for (const auto& key : requireInstanceUpdate) {
        auto& cachedGroup = mInstanceCache[key];
        if (!mInstancesBuffer.PrepareData(cachedGroup.instanceBufferRange,
                                          cachedGroup.instancesData)) {
            LOG_WARNING("Failed to prepare instance data!");
        }
    }

    for (const auto& key : requireIndirectUpdate) {
        auto& cachedGroup = mInstanceCache[key];
        Core::SIndirectDrawCommand cmd{};
        cmd.indexCount = static_cast<uint32_t>(
            mMeshDrawRanges[key.first].indexesRange.count);
        cmd.instanceCount =
            static_cast<uint32_t>(cachedGroup.instancesData.size());
        cmd.firstIndex = static_cast<uint32_t>(
            mMeshDrawRanges[key.first].indexesRange.first);
        cmd.vertexOffset = static_cast<int32_t>(
            mMeshDrawRanges[key.first].verticesRange.first);
        cmd.firstInstance =
            static_cast<uint32_t>(cachedGroup.instanceBufferRange.first);

        if (!mIndirectDrawBuffer.PrepareData(cachedGroup.indirectBufferRange,
                                             cmd)) {
            LOG_WARNING("Failed to prepare indirect draw command!");
        }
    }
}

std::unordered_map<std::size_t, std::vector<Utils::SBufferIndexRange>>
CMeshRenderer::GetIndirectDrawRanges() const {
    std::unordered_map<std::size_t, std::vector<Utils::SBufferIndexRange>>
        drawRanges;
    for (const auto& [key, group] : mInstanceCache) {
        drawRanges[key.second].push_back(group.indirectBufferRange);
    }
    return drawRanges;
}

} // namespace Renderer
