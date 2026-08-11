#pragma once
#include "engine/core/Mesh.h"

namespace Core {
class CMeshManager;
class CMeshFactory {
public:
    CMeshFactory(CMeshManager& meshManager);

    SMesh CreateTriangle();
    SMesh CreateCube(float size);
    SMesh CreateQuad(float width, float height, float depth);
    SMesh LoadFromFile(std::string filePath);

private:
    CMeshManager& mMeshManager;
};
} // namespace Core
