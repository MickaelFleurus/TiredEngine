#pragma once

#include <limits>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vulkan/vulkan.h>

#include "engine/utils/StringId.h"

using IndexType = uint32_t;
constexpr IndexType INVALID_INDEX = std::numeric_limits<IndexType>::max();
namespace Core {

//! Buffer data type
struct alignas(16) SInstanceData {
    uint32_t transformIndex;
    uint32_t partOffsetIndex;
    uint32_t meshInfoIndex;
    uint32_t materialIndex;
    glm::vec4 baseColor{1.0f};
};

//! Buffer data type
struct alignas(16) SVertex {
    glm::vec3 position;
    float padding;
    glm::vec3 normal;
    float padding2;
    glm::vec2 uv;
    float padding3[2];
    glm::vec4 tangent = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
};

// Uploaded data to GPU buffers
struct SGlobalMeshBuffers {
    std::vector<SVertex> vertices;
    std::vector<uint32_t> indices;
};

namespace Meshes {

// One node in the asset's internal hierarchy
struct SNode {
    glm::mat4 local{1.0f};
    glm::mat4 world{1.0f};
    int32_t parentIdx = -1;
    uint32_t meshInfoIndex = INVALID_INDEX;
    uint32_t partOffsetIndex = INVALID_INDEX;
    CStringId name;
};

// Data in a glTF file, used to load everything
struct SCompositeAsset {
    // Breadth-First Search ordered nodes, parents are always before children
    std::vector<SNode> nodes;
    bool isStaticInternally = true;

    int32_t findNodeByName(CStringId name) const {
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (nodes[i].name == name)
                return (int32_t)i;
        }
        return -1;
    }
};

// Instance data, created when a mesh is registered
//! Buffer Data Type
struct alignas(16) SInfo {
    uint32_t firstIndex;
    uint32_t indexCount;
    int32_t vertexOffset;
    uint32_t materialIndex;
    glm::vec3 localBoundsCenter;
    float localBoundsRadius;
};

// Buffer data, represent the local matrice in the loaded mesh
//! Buffer data type
struct SGPUPartOffset {
    glm::mat4 offset;
};

// Data to draw the meshes
struct SMeshAssetRegistry {
    std::vector<SInfo> meshInfos;
    std::vector<SCompositeAsset> assets;
    std::vector<SGPUPartOffset> partOffsets;
};
} // namespace Meshes

//! Buffer data type
struct alignas(4) SIndirectDrawCommand {
    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t vertexOffset;
    uint32_t firstInstance;
};

//! Buffer data type
struct alignas(16) SMaterial {
    uint32_t albedoTexIndex;
    uint32_t normalTexIndex;
    uint32_t metalRoughTexIndex;
    float metallic;
    float roughness;
    float _pad[3];
};

// Draw data for UI and screen space stuff
//! Buffer data type
struct alignas(16) SScreenQuadInstance {
    glm::vec2 posMin, posMax;
    glm::vec2 uvMin, uvMax;
    glm::vec4 color;
    uint32_t texIndex;
    uint32_t _pad[3];
};

// Draw data for 2D elements in world space (billboards, sprites, etc...)
//! Buffer data type
struct alignas(16) SWorldQuadInstance {
    glm::vec3 worldPos;
    glm::vec2 size;
    glm::vec2 uvMin, uvMax;
    glm::vec4 color;
    uint32_t texIndex;
    uint32_t _pad[3];
};

namespace PC {
// Push constants for UI and screen space stuff
struct SUiPushConstants {
    VkDeviceAddress instances; // uiGlyphBuffer or uiSpriteBuffer address
    glm::vec2 screenSize;
    float msdfPxRange = 2.0f;
    float _pad;
};

// Push constants for world space 2D stuff
struct SWorldPushConstants {
    VkDeviceAddress instances; // worldGlyphBuffer or worldSpriteBuffer address
    glm::mat4 view;
    glm::mat4 proj;
    float msdfPxRange;
    float _pad[3];
};

// Push constants for culling and indirect draw resolution
struct alignas(16) SCullPushConstants {
    VkDeviceAddress meshInfoBuffer;
    VkDeviceAddress indirectOut;
    VkDeviceAddress drawCount;
    uint32_t meshCount;
    uint32_t _pad0, _pad1, _pad2;
    glm::vec4 frustumPlanes[6];
};

// Push constants for rendering 3D meshes
struct alignas(16) SScenePushConstants {
    VkDeviceAddress vertexBuffer;
    VkDeviceAddress materialBuffer;
    VkDeviceAddress meshInfoBuffer;
    VkDeviceAddress objectInstanceBuffer;
};
} // namespace PC

} // namespace Core
