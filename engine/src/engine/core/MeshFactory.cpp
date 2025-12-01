#include "engine/core/MeshFactory.h"

#include "engine/core/DataTypes.h"
#include "engine/core/MeshManager.h"
#include "engine/utils/Hashing.h"

#include <array>

namespace {

constexpr std::array<Core::SVertex, 24>
Generate3DRectangleVertices(float width, float height, float depth) {
    return std::array<Core::SVertex, 24>{
        // Front face
        Core::SVertex{.position{-0.5f * width, -0.5f * height, 0.5f * depth},
                      .texCoord{0.0f, 0.0f},
                      .normal{0.0f, 0.0f, 1.0f}},
        Core::SVertex{.position{0.5f * width, -0.5f * height, 0.5f * depth},
                      .texCoord{1.0f, 0.0f},
                      .normal{0.0f, 0.0f, 1.0f}},
        Core::SVertex{.position{0.5f * width, 0.5f * height, 0.5f * depth},
                      .texCoord{1.0f, 1.0f},
                      .normal{0.0f, 0.0f, 1.0f}},
        Core::SVertex{.position{-0.5f * width, 0.5f * height, 0.5f * depth},
                      .texCoord{0.0f, 1.0f},
                      .normal{0.0f, 0.0f, 1.0f}},

        // Back face
        Core::SVertex{.position{0.5f * width, -0.5f * height, -0.5f * depth},
                      .texCoord{0.0f, 0.0f},
                      .normal{0.0f, 0.0f, -1.0f}},
        Core::SVertex{.position{-0.5f * width, -0.5f * height, -0.5f * depth},
                      .texCoord{1.0f, 0.0f},
                      .normal{0.0f, 0.0f, -1.0f}},
        Core::SVertex{.position{-0.5f * width, 0.5f * height, -0.5f * depth},
                      .texCoord{1.0f, 1.0f},
                      .normal{0.0f, 0.0f, -1.0f}},
        Core::SVertex{.position{0.5f * width, 0.5f * height, -0.5f * depth},
                      .texCoord{0.0f, 1.0f},
                      .normal{0.0f, 0.0f, -1.0f}},

        // Top face
        Core::SVertex{.position{-0.5f * width, 0.5f * height, 0.5f * depth},
                      .texCoord{0.0f, 0.0f},
                      .normal{0.0f, 1.0f, 0.0f}},
        Core::SVertex{.position{0.5f * width, 0.5f * height, 0.5f * depth},
                      .texCoord{1.0f, 0.0f},
                      .normal{0.0f, 1.0f, 0.0f}},
        Core::SVertex{.position{0.5f * width, 0.5f * height, -0.5f * depth},
                      .texCoord{1.0f, 1.0f},
                      .normal{0.0f, 1.0f, 0.0f}},
        Core::SVertex{.position{-0.5f * width, 0.5f * height, -0.5f * depth},
                      .texCoord{0.0f, 1.0f},
                      .normal{0.0f, 1.0f, 0.0f}},

        // Bottom face
        Core::SVertex{.position{-0.5f * width, -0.5f * height, -0.5f * depth},
                      .texCoord{0.0f, 0.0f},
                      .normal{0.0f, -1.0f, 0.0f}},
        Core::SVertex{.position{0.5f * width, -0.5f * height, -0.5f * depth},
                      .texCoord{1.0f, 0.0f},
                      .normal{0.0f, -1.0f, 0.0f}},
        Core::SVertex{.position{0.5f * width, -0.5f * height, 0.5f * depth},
                      .texCoord{1.0f, 1.0f},
                      .normal{0.0f, -1.0f, 0.0f}},
        Core::SVertex{.position{-0.5f * width, -0.5f * height, 0.5f * depth},
                      .texCoord{0.0f, 1.0f},
                      .normal{0.0f, -1.0f, 0.0f}},

        // Right face
        Core::SVertex{.position{0.5f * width, -0.5f * height, 0.5f * depth},
                      .texCoord{0.0f, 0.0f},
                      .normal{1.0f, 0.0f, 0.0f}},
        Core::SVertex{.position{0.5f * width, -0.5f * height, -0.5f * depth},
                      .texCoord{1.0f, 0.0f},
                      .normal{1.0f, 0.0f, 0.0f}},
        Core::SVertex{.position{0.5f * width, 0.5f * height, -0.5f * depth},
                      .texCoord{1.0f, 1.0f},
                      .normal{1.0f, 0.0f, 0.0f}},
        Core::SVertex{.position{0.5f * width, 0.5f * height, 0.5f * depth},
                      .texCoord{0.0f, 1.0f},
                      .normal{1.0f, 0.0f, 0.0f}},

        // Left face
        Core::SVertex{.position{-0.5f * width, -0.5f * height, -0.5f * depth},
                      .texCoord{0.0f, 0.0f},
                      .normal{-1.0f, 0.0f, 0.0f}},
        Core::SVertex{.position{-0.5f * width, -0.5f * height, 0.5f * depth},
                      .texCoord{1.0f, 0.0f},
                      .normal{-1.0f, 0.0f, 0.0f}},
        Core::SVertex{.position{-0.5f * width, 0.5f * height, 0.5f * depth},
                      .texCoord{1.0f, 1.0f},
                      .normal{-1.0f, 0.0f, 0.0f}},
        Core::SVertex{.position{-0.5f * width, 0.5f * height, -0.5f * depth},
                      .texCoord{0.0f, 1.0f},
                      .normal{-1.0f, 0.0f, 0.0f}}};
}

constexpr std::array<Core::SVertex, 24> GenerateCubeVertices(float size) {
    return Generate3DRectangleVertices(size, size, size);
}

constexpr std::array<uint32_t, 36> kCubeIndices{
    0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
    12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};
} // namespace

namespace Core {

CMeshFactory::CMeshFactory() = default;

CMesh CMeshFactory::CreateTriangle() {
    std::array<Core::SVertex, 3> triangleVertices{
        Core::SVertex{.position{0.0f, 0.5f, 0.0f},
                      .texCoord{0.5f, 1.0f},
                      .normal{0.0f, 0.0f, 1.0f}},
        Core::SVertex{.position{-0.5f, -0.5f, 0.0f},
                      .texCoord{0.0f, 0.0f},
                      .normal{0.0f, 0.0f, 1.0f}},
        Core::SVertex{.position{0.5f, -0.5f, 0.0f},
                      .texCoord{1.0f, 0.0f},
                      .normal{0.0f, 0.0f, 1.0f}}};

    std::array<uint32_t, 3> triangleIndices{0, 1, 2};

    return CMesh{EMeshBaseType::Triangle, EMeshDynamicType::Static,
                 triangleVertices, triangleIndices};
}

CMesh CMeshFactory::CreateCube(float size) {
    auto vertices = GenerateCubeVertices(size);
    return CMesh{
        EMeshBaseType::Cube, EMeshDynamicType::Static, vertices,
        std::span<const uint32_t>(kCubeIndices.data(), kCubeIndices.size())};
}

CMesh CMeshFactory::CreateQuad(float width, float height, float depth) {
    auto vertices = Generate3DRectangleVertices(width, height, depth);

    return CMesh{
        EMeshBaseType::Cube, EMeshDynamicType::Static, vertices,
        std::span<const uint32_t>(kCubeIndices.data(), kCubeIndices.size())};
}

// CMesh CMeshFactory::LoadFromFile(std::string filePath) {
//     // TODO
//     return nullptr;
// }
} // namespace Core