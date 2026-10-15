#pragma once
#include <cstdint>

#include <glm/vec2.hpp>

namespace Core {
struct SSpriteRendering {
    uint64_t id = -1;
    glm::vec2 uvMin{};
    glm::vec2 uvMax{};
};

} // namespace Core
