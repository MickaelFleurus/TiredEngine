#pragma once
#include <unordered_map>

#include "engine/core/Mesh.h"
#include "engine/core/MeshFactory.h"
#include "engine/utils/StringId.h"

namespace Core {

class CMeshManager {
public:
    CMeshManager();

    bool HasMesh(CStringId id) const;
    SMesh GetMesh(CStringId id);
    void ClearMeshes();

private:
    CMeshFactory mFactory;
    std::unordered_map<CStringId, SMesh, CStringIdHash> mMeshes;
};

} // namespace Core
