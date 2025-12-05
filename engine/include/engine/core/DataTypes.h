#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Core {

using IndexType = uint32_t;

struct SVertex {
    glm::vec3 position;
    float padding1;
    glm::vec2 texCoord;
    float padding2[2];
    glm::vec3 normal;
    float padding3;
};

struct SInstanceData {
    glm::mat4 modelMatrix;
    glm::vec4 color;
    int materialId;
    int textureId;
    float padding[2];
};

struct SIndirectDrawCommand {
    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t vertexOffset;
    uint32_t firstInstance;
};

struct STextInstanceData {
    glm::mat4 modelMatrix;
    glm::vec4 color;
    glm::vec4 uvRect;
    uint32_t materialId;
    uint32_t textureId;
    float padding[2];
};

struct SPushConstantData {
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};

} // namespace Core