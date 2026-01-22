#pragma once
#include "engine/core/Mesh.h"

namespace Core {

class CMeshFactory {
public:
    CMeshFactory();

    CMesh CreateTriangle(std::size_t hash);
    CMesh CreateCube(float size, std::size_t hash);
    CMesh CreateQuad(float width, float height, float depth, std::size_t hash);
    // CMesh LoadFromFile(std::string filePath);

private:
};
} // namespace Core
