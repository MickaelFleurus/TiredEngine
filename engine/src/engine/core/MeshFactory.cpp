#include "engine/core/MeshFactory.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <format>
#include <optional>
#include <queue>

#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include "engine/core/DataTypes.h"
#include "engine/utils/Logger.h"
#include "engine/utils/StringId.h"

namespace {

glm::mat4 ExtractLocalTransform(const fastgltf::Node& node) {
    return std::visit(
        fastgltf::visitor{
            [](const fastgltf::math::fmat4x4& m) {
                return glm::make_mat4(m.data());
            },
            [](const fastgltf::TRS& trs) {
                glm::mat4 t = glm::translate(
                    glm::mat4(1.0f), glm::make_vec3(trs.translation.data()));
                glm::mat4 r =
                    glm::mat4_cast(glm::quat(trs.rotation[3], trs.rotation[0],
                                             trs.rotation[1], trs.rotation[2]));
                glm::mat4 s = glm::scale(glm::mat4(1.0f),
                                         glm::make_vec3(trs.scale.data()));
                return t * r * s;
            }},
        node.transform);
}

std::expected<fastgltf::Asset, std::string>
ParseFile(std::filesystem::path path) {
    fastgltf::Parser parser;

    auto gltfFile = fastgltf::MappedGltfFile::FromPath(path);
    if (!bool(gltfFile)) {
        return std::unexpected(std::format(
            "File {} does not exists or could not be opened.", path.string()));
    }

    constexpr auto options =
        fastgltf::Options::DontRequireValidAssetMember |
        fastgltf::Options::LoadExternalBuffers | // resolves .bin for .gltf
        fastgltf::Options::LoadExternalImages |  // resolves external textures
        fastgltf::Options::GenerateMeshIndices;  // in case a primitive has no
                                                 // indices

    auto asset = parser.loadGltf(gltfFile.get(), path.parent_path(), options);
    if (auto err = asset.error(); err != fastgltf::Error::None) {
        return std::unexpected(
            std::format("Error while loading the file: {}",
                        static_cast<std::size_t>(asset.error())));
    }

    return std::move(asset.get());
}

std::unordered_map<std::size_t, std::vector<uint32_t>>
LoadMeshInfo(fastgltf::Asset& asset, Core::SGlobalMeshBuffers staging,
             std::vector<Core::Meshes::SInfo>& meshInfos) {

    std::unordered_map<std::size_t, std::vector<uint32_t>> meshToIndice;

    for (std::size_t id = 0; id < asset.meshes.size(); ++id) {
        auto& mesh = asset.meshes[id];

        for (fastgltf::Primitive& prim : mesh.primitives) {
            Core::Meshes::SInfo info{};
            info.vertexOffset = static_cast<uint32_t>(staging.vertices.size());
            info.firstIndex = static_cast<uint32_t>(staging.indices.size());

            auto* posIt = prim.findAttribute("POSITION");
            auto& posAccessor = asset.accessors[posIt->accessorIndex];
            std::size_t baseVertex = staging.vertices.size();
            staging.vertices.resize(baseVertex + posAccessor.count);

            glm::vec3 boundsMin(FLT_MAX), boundsMax(-FLT_MAX);
            fastgltf::iterateAccessorWithIndex<glm::vec3>(
                asset, posAccessor, [&](glm::vec3 pos, std::size_t i) {
                    staging.vertices[baseVertex + i].position = pos;
                    boundsMin = glm::min(boundsMin, pos);
                    boundsMax = glm::max(boundsMax, pos);
                });

            if (auto* nIt = prim.findAttribute("NORMAL");
                nIt != prim.attributes.end()) {
                fastgltf::iterateAccessorWithIndex<glm::vec3>(
                    asset, asset.accessors[nIt->accessorIndex],
                    [&](glm::vec3 n, std::size_t i) {
                        staging.vertices[baseVertex + i].normal = n;
                    });
            }
            if (auto* uvIt = prim.findAttribute("TEXCOORD_0");
                uvIt != prim.attributes.end()) {
                fastgltf::iterateAccessorWithIndex<glm::vec2>(
                    asset, asset.accessors[uvIt->accessorIndex],
                    [&](glm::vec2 uv, std::size_t i) {
                        staging.vertices[baseVertex + i].uv = uv;
                    });
            }

            if (prim.indicesAccessor.has_value()) {
                auto& idxAccessor =
                    asset.accessors[prim.indicesAccessor.value()];
                info.indexCount = static_cast<uint32_t>(idxAccessor.count);
                fastgltf::iterateAccessorWithIndex<uint32_t>(
                    asset, idxAccessor, [&](uint32_t idx, std::size_t) {
                        staging.indices.push_back(
                            static_cast<uint32_t>(baseVertex + idx));
                    });
            }

            info.materialIndex =
                prim.materialIndex.has_value()
                    ? static_cast<uint32_t>(prim.materialIndex.value())
                    : INVALID_INDEX;
            info.localBoundsCenter = (boundsMin + boundsMax) * 0.5f;
            info.localBoundsRadius =
                glm::length(boundsMax - info.localBoundsCenter);

            auto infoIdx = static_cast<uint32_t>(meshInfos.size());
            meshInfos.push_back(info);
            meshToIndice[id].push_back(infoIdx);
        }
    }
    return meshToIndice;
}

// Order them parents to children, compute the world matrice
Core::Meshes::SCompositeAsset BuildCompositeMeshAsset(
    fastgltf::Asset& gltf, std::size_t sceneIndex,
    const std::unordered_map<std::size_t, std::vector<uint32_t>>&
        gltfMeshToInfoIndices) {
    Core::Meshes::SCompositeAsset asset;
    std::queue<std::pair<std::size_t, int32_t>> q;
    for (std::size_t rootIdx : gltf.scenes[sceneIndex].nodeIndices) {
        q.push({rootIdx, -1});
    }

    while (!q.empty()) {
        auto [gltfIdx, parentLocalIdx] = q.front();
        q.pop();
        fastgltf::Node& gnode = gltf.nodes[gltfIdx];

        glm::mat4 local = ExtractLocalTransform(gnode);

        auto meshIt = gnode.meshIndex.has_value()
                          ? gltfMeshToInfoIndices.find(gnode.meshIndex.value())
                          : gltfMeshToInfoIndices.end();

        int32_t firstCreatedIdx = -1;
        if (meshIt != gltfMeshToInfoIndices.end()) {
            for (uint32_t infoIdx : meshIt->second) {
                Core::Meshes::SNode node;
                node.local = local;
                node.parentIdx = parentLocalIdx;
                node.meshInfoIndex = infoIdx;
                node.name = CStringId(gnode.name.c_str());
                if (firstCreatedIdx < 0)
                    firstCreatedIdx = (int32_t)asset.nodes.size();
                asset.nodes.push_back(node);
            }
        } else {
            Core::Meshes::SNode node;
            node.local = local;
            node.parentIdx = parentLocalIdx;
            node.name = CStringId(gnode.name.c_str());
            firstCreatedIdx = (int32_t)asset.nodes.size();
            asset.nodes.push_back(node);
        }

        for (std::size_t childIdx : gnode.children) {
            q.push({childIdx, firstCreatedIdx});
        }
    }

    for (auto& node : asset.nodes) {
        node.world = (node.parentIdx >= 0)
                         ? asset.nodes[node.parentIdx].world * node.local
                         : node.local;
    }

    return asset;
}

std::expected<uint32_t, std::string>
LoadGltfAsset(const std::filesystem::path& path,
              Core::Meshes::SMeshAssetRegistry& registry,
              Core::SGlobalMeshBuffers& staging) {
    auto gltf = ParseFile(path);
    if (!gltf.has_value()) {
        return std::unexpected(gltf.error());
    }

    auto meshMap = LoadMeshInfo(
        *gltf, staging,
        registry.meshInfos); // Mesh info loading, submesh by submesh in the
    Core::Meshes::SCompositeAsset asset =
        BuildCompositeMeshAsset(*gltf, gltf->defaultScene.value_or(0), meshMap);

    // register baked part offsets for every drawable node
    for (Core::Meshes::SNode& node : asset.nodes) {
        if (node.meshInfoIndex == INVALID_INDEX)
            continue;

        node.partOffsetIndex =
            static_cast<uint32_t>(registry.partOffsets.size());
        registry.partOffsets.push_back({node.world});
    }

    uint32_t assetIndex = static_cast<uint32_t>(registry.assets.size());
    registry.assets.push_back(std::move(asset));
    return assetIndex;
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

void CMeshFactory::StartMassLoad() {
    mStagingBuffer.indices.clear();
    mStagingBuffer.vertices.clear();
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

std::optional<SMesh>
CMeshFactory::LoadFromFile(const std::filesystem::path& filePath) {
    auto loaded = LoadGltfAsset(filePath, mRegistry, mStagingBuffer);
    if (!loaded.has_value()) {
        LOG_ERROR("Could not load file {}: {}", filePath.c_str(),
                  loaded.error().c_str());
        return std::nullopt;
    };

    return SMesh{CStringId{filePath}, *loaded};
}
} // namespace Core
