#include "engine/renderer/MeshRenderer.h"

#include "engine/core/Mesh.h"
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

    if (renderables.empty()) {
        LOG_WARNING("No renderables to process!");
        return;
    }

    // Build map of current renderables by key
    std::unordered_map<std::pair<std::size_t, std::size_t>,
                       std::vector<Core::SInstanceData>, SPairHash>
        newInstancesByKey;
    std::unordered_map<std::pair<std::size_t, std::size_t>,
                       std::vector<Core::GameObjectId>, SPairHash>
        newGameObjectsByKey;

    for (const auto& renderable : renderables) {
        auto key = std::make_pair(renderable.meshHash, renderable.materialId);
        newInstancesByKey[key].push_back(Core::SInstanceData{
            .modelMatrix = renderable.transform,
            .color = renderable.color,
            .materialId = renderable.materialId,
            .textureId = renderable.textureIndex,
        });
        newGameObjectsByKey[key].push_back(renderable.id);
    }

    // Find removed instances by comparing old and new
    // for (auto it = mInstanceCache.begin(); it != mInstanceCache.end();) {
    //     const auto& key = it->first;
    //     auto& cachedGroup = it->second;

    //     if (newGameObjectsByKey.find(key) == newGameObjectsByKey.end()) {
    //         // Entire group was removed
    //         mInstancesBuffer.FreeRange(cachedGroup.instanceBufferRange);
    //         mIndirectDrawBuffer.FreeRange(cachedGroup.indirectBufferRange);
    //         it = mInstanceCache.erase(it);
    //     } else {
    //         // Check if specific instances were removed
    //         const auto& newGameObjects = newGameObjectsByKey[key];
    //         const auto& oldGameObjects = cachedGroup.gameObjectIds;

    //         bool hasRemovals = false;
    //         for (const auto& oldId : oldGameObjects) {
    //             if (std::find(newGameObjects.begin(), newGameObjects.end(),
    //                           oldId) == newGameObjects.end()) {
    //                 LOG_INFO("GameObjectId {} removed from rendering",
    //                 oldId); hasRemovals = true; break;
    //             }
    //         }

    //         // If removals detected, free old buffer ranges
    //         if (hasRemovals) {
    //             mInstancesBuffer.FreeRange(cachedGroup.instanceBufferRange);
    //             mIndirectDrawBuffer.FreeRange(cachedGroup.indirectBufferRange);
    //             cachedGroup.instanceBufferRange = {};
    //             cachedGroup.indirectBufferRange = {};
    //         }

    //         ++it;
    //     }
    // }

    // Update cache with new instances
    for (auto& [key, instancesData] : newInstancesByKey) {
        auto& cachedGroup = mInstanceCache[key];
        cachedGroup.gameObjectIds = newGameObjectsByKey[key];

        const auto oldSize = cachedGroup.instancesData.size();
        const auto newSize = instancesData.size();
        bool dataChanged = (oldSize != newSize) ||
                           (cachedGroup.instancesData != instancesData);

        if (dataChanged) {
            cachedGroup.instancesData = std::move(instancesData);

            if (!mInstancesBuffer.PrepareData(cachedGroup.instanceBufferRange,
                                              cachedGroup.instancesData)) {
                LOG_WARNING("Failed to prepare instance data!");
            }

            if (oldSize != newSize) {
                Core::SIndirectDrawCommand cmd{};
                cmd.indexCount = static_cast<uint32_t>(
                    mMeshDrawRanges[key.first].indexesRange.count);
                cmd.instanceCount = newSize;
                cmd.firstIndex = static_cast<uint32_t>(
                    mMeshDrawRanges[key.first].indexesRange.first);
                cmd.vertexOffset = static_cast<int32_t>(
                    mMeshDrawRanges[key.first].verticesRange.first);
                cmd.firstInstance = static_cast<uint32_t>(
                    cachedGroup.instanceBufferRange.first);

                if (!mIndirectDrawBuffer.PrepareData(
                        cachedGroup.indirectBufferRange, cmd)) {
                    LOG_WARNING("Failed to prepare "
                                "indirect draw "
                                "command!");
                }
            }
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
