#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Renderer {

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

struct STextInstanceData {
    glm::mat4 modelMatrix;
    glm::vec4 color;
    glm::vec4 uvRect;
    int materialId;
    int textureId;
    float padding[2];
};

struct SPushConstantData {
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};

} // namespace Renderer