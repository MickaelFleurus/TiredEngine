#pragma once
#include <optional>
#include <vector>

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "engine/core/GameObjectId.h"

namespace Renderer {
struct STransformBase {
    glm::mat4 mLocal;
    glm::mat4 mWorld;
};

struct STransformExtended {
    glm::vec3 mPosition;
    glm::vec3 mScale;
    glm::quat mRotation;
};

struct STransformExtra {
    std::optional<Core::GameObjectId> mParent{std::nullopt};
    std::vector<Core::GameObjectId> mChildren;
    bool mDirty{false};
};
} // namespace Renderer
