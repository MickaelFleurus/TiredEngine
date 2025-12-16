#include "engine/core/MeshManager.h"

#include "engine/core/Mesh.h"
#include "engine/renderer/MeshRenderer.h"
#include "engine/utils/Hashing.h"

namespace Core {

CMeshManager::CMeshManager(Renderer::CMeshRenderer& meshRenderer)
    : mMeshRenderer(meshRenderer) {
}

CMeshManager::~CMeshManager() = default;

bool CMeshManager::HasMesh(std::size_t meshHash) const {
    return mMeshes.contains(meshHash);
}

CMesh* CMeshManager::GetMesh(std::size_t meshHash) {
    return &mMeshes.at(meshHash);
}

void CMeshManager::ClearMeshes() {
    mMeshes.clear();
}

CMesh* CMeshManager::CreateCube(float size) {
    std::size_t meshHash =
        Utils::CreateHash(EMeshBaseType::Cube, EMeshDynamicType::Static, size);
    if (!HasMesh(meshHash)) {
        mMeshes.emplace(meshHash, std::move(mFactory.CreateCube(
                                      size, meshHash, mMeshRenderer)));
    }
    return &mMeshes.at(meshHash);
}

CMesh* CMeshManager::CreateTriangle() {
    std::size_t meshHash =
        Utils::CreateHash(EMeshBaseType::Triangle, EMeshDynamicType::Static);
    if (!HasMesh(meshHash)) {
        auto mesh = mFactory.CreateTriangle(meshHash, mMeshRenderer);
        mMeshes.emplace(meshHash, std::move(mesh));
    }
    return &mMeshes.at(meshHash);
}

} // namespace Core
