#pragma once
#include <cstdint>
#include <unordered_map>

#include <glm/vec2.hpp>

#include "engine/utils/StringId.h"

namespace Core {
struct SSpriteRegion {
    glm::vec2 uvMin{};
    glm::vec2 uvMax{};
};

struct SSpriteAtlas {
    uint64_t id = -1;
    std::unordered_map<CStringId, SSpriteRegion, CStringIdHash> sprites;
};
} // namespace Core
