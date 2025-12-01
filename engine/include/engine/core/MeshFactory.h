#pragma once
#include "engine/core/Mesh.h"

namespace Core {

class CMeshFactory {
public:
    CMeshFactory();

    CMesh CreateTriangle();
    CMesh CreateCube(float size);
    CMesh CreateQuad(float width, float height, float depth);
    // CMesh LoadFromFile(std::string filePath);

private:
};
} // namespace Core