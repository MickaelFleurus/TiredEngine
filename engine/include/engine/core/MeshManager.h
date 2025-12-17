#pragma once
#include "engine/core/Mesh.h"
#include "engine/core/MeshFactory.h"

#include <unordered_map>

namespace Renderer {
class CMeshRenderer;
} // namespace Renderer

namespace Core {

class CMeshManager {
public:
    CMeshManager(Renderer::CMeshRenderer& meshRenderer);
    ~CMeshManager();

    bool HasMesh(std::size_t meshHash) const;
    CMesh* GetMesh(std::size_t meshHash);
    void ClearMeshes();

    CMesh* CreateCube(float size);
    CMesh* CreateTriangle();

private:
    CMeshFactory mFactory;
    Renderer::CMeshRenderer& mMeshRenderer;
    std::unordered_map<std::size_t, CMesh> mMeshes;
};

} // namespace Core
