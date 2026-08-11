#include "engine/core/MeshFactory.h"

#include <array>

#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include "engine/core/DataTypes.h"

namespace {

bool Load(std::filesystem::path path) {
    fastgltf::Parser parser;

    auto gltfFile = fastgltf::MappedGltfFile::FromPath(path);
    if (!bool(gltfFile)) {
        // gltfFile.error() -> fastgltf::Error
        return false;
    }

    constexpr auto options =
        fastgltf::Options::DontRequireValidAssetMember |
        fastgltf::Options::LoadExternalBuffers | // resolves .bin for .gltf
        fastgltf::Options::LoadExternalImages |  // resolves external textures
        fastgltf::Options::GenerateMeshIndices;  // in case a primitive has no
                                                 // indices

    auto asset = parser.loadGltf(gltfFile.get(), path.parent_path(), options);
    if (auto err = asset.error(); err != fastgltf::Error::None) {
        // handle error
        return false;
    }

    fastgltf::Asset& gltf = asset.get();
    for (fastgltf::Mesh& mesh : gltf.meshes) {
        for (fastgltf::Primitive& prim : mesh.primitives) {
            std::vector<Core::SVertex> vertices;
            std::vector<uint32_t> indices;

            // --- Positions (always present) ---
            auto* posIt = prim.findAttribute("POSITION");
            auto& posAccessor = gltf.accessors[posIt->accessorIndex];
            vertices.resize(posAccessor.count);

            fastgltf::iterateAccessorWithIndex<glm::vec3>(
                gltf, posAccessor, [&](glm::vec3 pos, size_t idx) {
                    vertices[idx].position = pos;
                });

            // --- Normals (optional) ---
            if (auto* nIt = prim.findAttribute("NORMAL");
                nIt != prim.attributes.end()) {
                auto& nAccessor = gltf.accessors[nIt->accessorIndex];
                fastgltf::iterateAccessorWithIndex<glm::vec3>(
                    gltf, nAccessor,
                    [&](glm::vec3 n, size_t idx) { vertices[idx].normal = n; });
            }

            // --- UVs (optional) ---
            if (auto* uvIt = prim.findAttribute("TEXCOORD_0");
                uvIt != prim.attributes.end()) {
                auto& uvAccessor = gltf.accessors[uvIt->accessorIndex];
                fastgltf::iterateAccessorWithIndex<glm::vec2>(
                    gltf, uvAccessor,
                    [&](glm::vec2 uv, size_t idx) { vertices[idx].uv = uv; });
            }

            // --- Indices ---
            if (prim.indicesAccessor.has_value()) {
                auto& idxAccessor =
                    gltf.accessors[prim.indicesAccessor.value()];
                indices.resize(idxAccessor.count);
                fastgltf::iterateAccessorWithIndex<uint32_t>(
                    gltf, idxAccessor,
                    [&](uint32_t idx, size_t i) { indices[i] = idx; });
            }

            for (fastgltf::Material& mat : gltf.materials) {
                auto& pbr = mat.pbrData;
                glm::vec4 baseColor = {
                    pbr.baseColorFactor[0], pbr.baseColorFactor[1],
                    pbr.baseColorFactor[2], pbr.baseColorFactor[3]};
                float metallic = pbr.metallicFactor;
                float roughness = pbr.roughnessFactor;

                int baseColorTexIdx =
                    pbr.baseColorTexture.has_value()
                        ? (int)pbr.baseColorTexture->textureIndex
                        : -1;
                int normalTexIdx = mat.normalTexture.has_value()
                                       ? (int)mat.normalTexture->textureIndex
                                       : -1;
            }
            for (fastgltf::Image& image : gltf.images) {
                std::visit(
                    fastgltf::visitor{[](fastgltf::sources::URI& uri) {
                                          // load file from disk yourself
                                          // (stb_image, etc.)
                                      },
                                      [](fastgltf::sources::Array& vec) {
                                          // decode vec.bytes with
                                          // stb_image::stbi_load_from_memory
                                      },
                                      [](fastgltf::sources::BufferView& view) {
                                          // resolve through
                                          // gltf.bufferViews/buffers, then
                                          // decode
                                      },
                                      [](auto&) { /* other variants */ }},
                    image.data);
            }

            for (fastgltf::Node& node : gltf.nodes) {
                glm::mat4 local = std::visit(
                    fastgltf::visitor{
                        [](fastgltf::math::fmat4x4& m) {
                            return glm::make_mat4(m.data());
                        },
                        [](fastgltf::TRS& trs) {
                            glm::mat4 t = glm::translate(
                                glm::mat4(1),
                                glm::make_vec3(trs.translation.data()));
                            glm::mat4 r = glm::mat4_cast(
                                glm::quat(trs.rotation[3], trs.rotation[0],
                                          trs.rotation[1], trs.rotation[2]));
                            glm::mat4 s = glm::scale(
                                glm::mat4(1), glm::make_vec3(trs.scale.data()));
                            return t * r * s;
                        }},
                    node.transform);

                if (node.meshIndex.has_value()) {
                }
            }
        }
    }
}

constexpr std::array<Core::SVertex, 24>
Generate3DRectangleVertices(float width, float height, float depth) {
    return std::array<Core::SVertex, 24>{
        // Front face
        Core::SVertex{.position{-0.5f * width, -0.5f * height, 0.5f * depth},
                      .normal{0.0f, 0.0f, 1.0f},
                      .uv{0.0f, 0.0f}},
        Core::SVertex{.position{0.5f * width, -0.5f * height, 0.5f * depth},
                      .normal{0.0f, 0.0f, 1.0f},
                      .uv{1.0f, 0.0f}},
        Core::SVertex{.position{0.5f * width, 0.5f * height, 0.5f * depth},
                      .normal{0.0f, 0.0f, 1.0f},
                      .uv{1.0f, 1.0f}},
        Core::SVertex{.position{-0.5f * width, 0.5f * height, 0.5f * depth},
                      .normal{0.0f, 0.0f, 1.0f},
                      .uv{0.0f, 1.0f}},

        // Back face
        Core::SVertex{.position{0.5f * width, -0.5f * height, -0.5f * depth},
                      .normal{0.0f, 0.0f, -1.0f},
                      .uv{0.0f, 0.0f}},
        Core::SVertex{.position{-0.5f * width, -0.5f * height, -0.5f * depth},
                      .normal{0.0f, 0.0f, -1.0f},
                      .uv{1.0f, 0.0f}},
        Core::SVertex{.position{-0.5f * width, 0.5f * height, -0.5f * depth},
                      .normal{0.0f, 0.0f, -1.0f},
                      .uv{1.0f, 1.0f}},
        Core::SVertex{.position{0.5f * width, 0.5f * height, -0.5f * depth},
                      .normal{0.0f, 0.0f, -1.0f},
                      .uv{0.0f, 1.0f}},

        // Top face
        Core::SVertex{.position{-0.5f * width, 0.5f * height, 0.5f * depth},
                      .normal{0.0f, 1.0f, 0.0f},
                      .uv{0.0f, 0.0f}},
        Core::SVertex{.position{0.5f * width, 0.5f * height, 0.5f * depth},
                      .normal{0.0f, 1.0f, 0.0f},
                      .uv{1.0f, 0.0f}},
        Core::SVertex{.position{0.5f * width, 0.5f * height, -0.5f * depth},
                      .normal{0.0f, 1.0f, 0.0f},
                      .uv{1.0f, 1.0f}},
        Core::SVertex{.position{-0.5f * width, 0.5f * height, -0.5f * depth},
                      .normal{0.0f, 1.0f, 0.0f},
                      .uv{0.0f, 1.0f}},

        // Bottom face
        Core::SVertex{.position{-0.5f * width, -0.5f * height, -0.5f * depth},
                      .normal{0.0f, -1.0f, 0.0f},
                      .uv{0.0f, 0.0f}},
        Core::SVertex{.position{0.5f * width, -0.5f * height, -0.5f * depth},
                      .normal{0.0f, -1.0f, 0.0f},
                      .uv{1.0f, 0.0f}},
        Core::SVertex{.position{0.5f * width, -0.5f * height, 0.5f * depth},
                      .normal{0.0f, -1.0f, 0.0f},
                      .uv{1.0f, 1.0f}},
        Core::SVertex{.position{-0.5f * width, -0.5f * height, 0.5f * depth},
                      .normal{0.0f, -1.0f, 0.0f},
                      .uv{0.0f, 1.0f}},

        // Right face
        Core::SVertex{.position{0.5f * width, -0.5f * height, 0.5f * depth},
                      .normal{1.0f, 0.0f, 0.0f},
                      .uv{0.0f, 0.0f}},
        Core::SVertex{.position{0.5f * width, -0.5f * height, -0.5f * depth},
                      .normal{1.0f, 0.0f, 0.0f},
                      .uv{1.0f, 0.0f}},
        Core::SVertex{.position{0.5f * width, 0.5f * height, -0.5f * depth},
                      .normal{1.0f, 0.0f, 0.0f},
                      .uv{1.0f, 1.0f}},
        Core::SVertex{.position{0.5f * width, 0.5f * height, 0.5f * depth},
                      .normal{1.0f, 0.0f, 0.0f},
                      .uv{0.0f, 1.0f}},

        // Left face
        Core::SVertex{.position{-0.5f * width, -0.5f * height, -0.5f * depth},
                      .normal{-1.0f, 0.0f, 0.0f},
                      .uv{0.0f, 0.0f}},
        Core::SVertex{.position{-0.5f * width, -0.5f * height, 0.5f * depth},
                      .normal{-1.0f, 0.0f, 0.0f},
                      .uv{1.0f, 0.0f}},
        Core::SVertex{.position{-0.5f * width, 0.5f * height, 0.5f * depth},
                      .normal{-1.0f, 0.0f, 0.0f},
                      .uv{1.0f, 1.0f}},
        Core::SVertex{.position{-0.5f * width, 0.5f * height, -0.5f * depth},
                      .normal{-1.0f, 0.0f, 0.0f},
                      .uv{0.0f, 1.0f}}};
}

constexpr std::array<Core::SVertex, 24> GenerateCubeVertices(float size) {
    return Generate3DRectangleVertices(size, size, size);
}

constexpr std::array<uint32_t, 36> kCubeIndices{
    0,  3,  2,  2,  1,  0,  4,  7,  6,  6,  5,  4,  8,  11, 10, 10, 9,  8,
    12, 15, 14, 14, 13, 12, 16, 19, 18, 18, 17, 16, 20, 23, 22, 22, 21, 20};
} // namespace

namespace Core {

CMeshFactory::CMeshFactory(CMeshManager& meshManager)
    : mMeshManager(meshManager) {
}

SMesh CMeshFactory::CreateTriangle() {
    std::array<Core::SVertex, 3> triangleVertices{
        Core::SVertex{.position{0.0f, 0.5f, 0.0f},
                      .normal{0.0f, 0.0f, 1.0f},
                      .uv{0.5f, 1.0f}},
        Core::SVertex{.position{-0.5f, -0.5f, 0.0f},
                      .normal{0.0f, 0.0f, 1.0f},
                      .uv{0.0f, 0.0f}},
        Core::SVertex{.position{0.5f, -0.5f, 0.0f},
                      .normal{0.0f, 0.0f, 1.0f},
                      .uv{1.0f, 0.0f}}};

    std::array<uint32_t, 3> triangleIndices{0, 2, 1};

    return SMesh{CStringId{"triangle"}, 0};
}

SMesh CMeshFactory::CreateCube(float size) {
    auto vertices = GenerateCubeVertices(size);
    return SMesh{CStringId{"cube"}, 0};
}

SMesh CMeshFactory::CreateQuad(float width, float height, float depth) {
    auto vertices = Generate3DRectangleVertices(width, height, depth);

    return SMesh{CStringId{"quad"}, 0};
}

SMesh CMeshFactory::LoadFromFile(std::string filePath) {
    // TODO
    return SMesh{CStringId{"file"}, 0};
}
} // namespace Core
