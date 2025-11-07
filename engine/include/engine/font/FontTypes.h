#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace Font {
struct GlyphInfo {
    glm::vec4 uv{};
    float advance;
    glm::vec2 offset{};
    glm::vec2 size{};
};

} // namespace Font