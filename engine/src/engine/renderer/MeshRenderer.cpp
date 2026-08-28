// #include "engine/renderer/MeshRenderer.h"

// #include "engine/component/MeshComponent.h"
// #include "engine/core/Mesh.h"
// #include "engine/renderer/TransformManager.h"
// #include "engine/utils/Logger.h"
// #include "engine/vulkan/BufferHandler.h"

// #define GLM_ENABLE_EXPERIMENTAL
// #include <glm/gtx/string_cast.hpp>

// namespace Renderer {
// CMeshRenderer::CMeshRenderer(Vulkan::CBufferHandler& bufferHandler,
//                              CTransformManager& transformManager)
//     : mBufferHandler(bufferHandler), mTransformManager(transformManager) {
// }

// CMeshRenderer::~CMeshRenderer() = default;

// void CMeshRenderer::RegisterMesh(const Core::CMesh* mesh) {
//     if (mMeshDrawRanges.contains(mesh->GetHash())) {
//         return;
//     }

//     Utils::SBufferIndexRange verticesRange{};
//     Utils::SBufferIndexRange indexesRange{};
//     bool dataPrepared =
//         mVertexBufferHandle.PrepareData(verticesRange, mesh->GetVertices());
//     dataPrepared = dataPrepared && mIndexesBufferHandle.PrepareData(
//                                        indexesRange, mesh->GetIndexes());

//     if (!dataPrepared) {
//         LOG_FATAL(
//             "Failed to register mesh! This is the symptom of too many unique
//             " "meshes being created or a buffer being way too small.");
//         return;
//     }

//     mMeshDrawRanges.emplace(mesh->GetHash(),
//                             SMeshDrawData{.verticesRange = verticesRange,
//                                           .indexesRange = indexesRange});
// }

// void CMeshRenderer::UnregisterMesh(const Core::CMesh* mesh) {
//     auto it = mMeshDrawRanges.find(mesh->GetHash());
//     if (it != mMeshDrawRanges.end()) {
//         const auto& drawData = it->second;
//         mVertexBufferHandle.FreeRange(drawData.verticesRange);
//         mIndexesBufferHandle.FreeRange(drawData.indexesRange);
//         mMeshDrawRanges.erase(it);
//     }
// }

// } // namespace Renderer
