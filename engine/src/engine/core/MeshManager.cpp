#include "engine/core/MeshManager.h"

#include "engine/core/Mesh.h"

namespace Core {

CMeshManager::CMeshManager() : mFactory(*this) {
}

bool CMeshManager::HasMesh(CStringId id) const {
    return mMeshes.contains(id);
}

SMesh CMeshManager::GetMesh(CStringId id) {
    return mMeshes.at(id);
}

void CMeshManager::ClearMeshes() {
    mMeshes.clear();
}

} // namespace Core
