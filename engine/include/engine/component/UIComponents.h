#pragma once
#include <cstddef>
#include <string>

#include <glm/vec4.hpp>

#include "engine/core/DataTypes.h"
#include "engine/core/Entity.h"

namespace Component {
struct SText {
    std::string content;
    uint32_t fontId;
    float fontSize{16.0f};
    glm::vec4 color{1.0f};
    bool isAnimated{false};

    std::vector<Core::SScreenQuadInstance>
        cachedGlyphs; // populated by build pass
    size_t lastContentHash{0};
    bool dirty{true};
};

struct SSprite {
    uint32_t texIndex;
    glm::vec2 uvMin, uvMax;
    glm::vec4 color{1.0f};
    glm::vec2 size;
};

struct SScreenTransform {
    glm::vec2 anchor{0.0f};
    glm::vec2 position{0.0f};
    float rotation = 0.0f;
    glm::vec2 scale{1.0f};
    glm::mat3 resolvedTransform; // computed each frame
};

struct SHierarchy {
    Core::SEntity parent = Core::kNullEntity;
    std::vector<Core::SEntity> children;
};
} // namespace Component
