#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/vec3.hpp>

#include "engine/core/Entity.h"

namespace Component {
struct SLocalTransform {
    glm::vec3 position{};
    glm::quat rotation{};
    glm::vec3 scale{};
};

struct SWorldTransform {
    glm::mat4 matrice{glm::identity<glm::mat4>()};
};

struct SParent {
    Core::SEntity entity;
};

} // namespace Component
