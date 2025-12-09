#pragma once
#include "engine/core/Mesh.h"

namespace Renderer {
class CMeshRenderer;
} // namespace Renderer

namespace Core {

class CMeshFactory {
public:
    CMeshFactory();

    CMesh CreateTriangle(std::size_t hash,
                         Renderer::CMeshRenderer& meshRenderer);
    CMesh CreateCube(float size, std::size_t hash,
                     Renderer::CMeshRenderer& meshRenderer);
    CMesh CreateQuad(float width, float height, float depth, std::size_t hash,
                     Renderer::CMeshRenderer& meshRenderer);
    // CMesh LoadFromFile(std::string filePath);

private:
};
} // namespace Core