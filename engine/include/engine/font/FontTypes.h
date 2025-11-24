#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Font {
struct GlyphInfo {
    glm::vec4 uv{};
    float advance;
    glm::vec2 offset{};
    glm::vec2 size{};
};

struct alignas(32) GlyphVertex {
    glm::vec3 position;
    float pad0;
    glm::vec2 uv;
    float pad1[2];
};

} // namespace Font