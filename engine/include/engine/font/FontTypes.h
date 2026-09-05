#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Font {
struct GlyphInfo {
    glm::vec4 planeBounds{}; // left, bottom, right, top in em space
    glm::vec2 uvMin{};
    glm::vec2 uvMax{};
    float advance;
};

} // namespace Font
