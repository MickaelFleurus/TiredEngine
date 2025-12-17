#pragma once
#include <glm/vec2.hpp>

namespace Core {
struct SSpriteInfo {
    int textureIndex = -1;
    glm::ivec2 origin{};
    glm::ivec2 size{};
};
} // namespace Core
