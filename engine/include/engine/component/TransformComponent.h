#pragma once

#include "engine/component/IComponent.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace Component {
class CTransformComponent : public IComponent {
public:
    CTransformComponent(Core::CGameObject& owner,
                        CComponentManager& componentManager);

    void update(float deltaTime) override;

    void setPosition(const glm::vec3& position);
    const glm::vec3& getPosition() const;

    void setRotation(const glm::vec3& rotation);
    glm::vec3 getRotation() const;

    void setScale(const glm::vec3& scale);
    const glm::vec3& getScale() const;

    void UpdateMatrix(glm::mat4& parentTransform) const;

private:
    glm::vec3 mPosition{0.f, 0.f, 0.f};
    glm::quat mRotation{1.f, 0.f, 0.f, 0.f};
    glm::vec3 mScale{1.f, 1.f, 1.f};
};
} // namespace Component