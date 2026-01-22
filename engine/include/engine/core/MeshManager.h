#pragma once
#include <unordered_map>

#include "engine/core/Mesh.h"
#include "engine/core/MeshFactory.h"

namespace Core {

class CMeshManager {
public:
    CMeshManager() = default;

    bool HasMesh(std::size_t meshHash) const;
    CMesh* GetMesh(std::size_t meshHash);
    void ClearMeshes();

    CMesh* CreateCube(float size);
    CMesh* CreateTriangle();

private:
    CMeshFactory mFactory;
    std::unordered_map<std::size_t, CMesh> mMeshes;
};

} // namespace Core
