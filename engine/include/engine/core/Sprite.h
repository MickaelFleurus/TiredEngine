#pragma once
#include <unordered_map>

#include <glm/vec2.hpp>

#include "engine/utils/StringId.h"

namespace Core {
struct SSpriteRegion {
    glm::vec2 uvMin{};
    glm::vec2 uvMax{};
};

struct SSpriteAtlas {
    int id = -1;
    std::unordered_map<CStringId, SSpriteRegion, CStringIdHash> sprites;
};
} // namespace Core
