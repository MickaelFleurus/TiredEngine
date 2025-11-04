#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Renderer {
struct SVertex {
    glm::vec3 position;
    glm::vec2 texCoord;
};

} // namespace Renderer