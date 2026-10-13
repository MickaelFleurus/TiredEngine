#pragma once
#include "engine/component/ComponentManager2.h"

#include "glm/fwd.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/vec3.hpp>

#include "engine/core/Entity.h"
#include "engine/utils/ComponentParserRegistry.h"

namespace Component {
struct SLocalTransform {
    glm::vec3 position{};
    glm::quat rotation{};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
};

struct SWorldTransform {
    glm::mat4 matrice{glm::identity<glm::mat4>()};
};

static ComponentRegistrar RegisterTransform(
    "Transform",
    [](Component::CManager& manager, Core::SEntity e, const nlohmann::json& j) {
        auto& transform = manager.Emplace<SLocalTransform>(e);
        if (j.contains("pos")) {
            auto& pos = j.at("pos");
            transform.position = glm::vec3(pos[0], pos[1], pos[2]);
        }
        if (j.contains("rot")) {
            auto& rot = j.at("rot");
            transform.rotation = glm::quat(rot[3], rot[0], rot[1], rot[2]);
        }
        if (j.contains("scale")) {
            auto& scale = j.at("scale");
            transform.scale = glm::vec3(scale[0], scale[1], scale[2]);
        }
    });

} // namespace Component
