#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

#include "engine/core/GameObjectId.h"

namespace Renderer {
class CTransformManager;
class CTransformHandle {
public:
    explicit CTransformHandle(CTransformManager& manager,
                              Core::GameObjectId id);

    void SetPosition(glm::vec3 position);
    glm::vec3 GetPosition() const;
    void Move(glm::vec3 move);

    void SetRotation(glm::vec3 rotation);
    glm::vec3 GetRotation() const;
    void SetScale(const glm::vec3& scale);
    glm::vec3 GetScale() const;

    bool HasChildren() const;

    const std::vector<Core::GameObjectId>& GetChildren() const;

private:
    Core::GameObjectId mId;
    CTransformManager& mManager;
};
} // namespace Renderer
