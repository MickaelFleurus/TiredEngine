#pragma once
#include "engine/component/IComponent.h"

#include <glm/vec3.hpp>

namespace Component {
class CMovementComponent : public IComponent {
public:
    CMovementComponent(Core::CGameObject& owner,
                       CComponentManager& componentManager, float acceleration);

    void Update(float deltaTime) override;

    void addDirection(glm::vec3 direction);
    void setDirection(glm::vec3 direction);
    const glm::vec3& getDirection() const;

    const glm::vec3& getVelocity() const;

    void applyPosition(const glm::vec3& position);

private:
    const float mAcceleration;

    glm::vec3 mDirection{0.f, 0.f, 0.f};
    glm::vec3 mVelocity{0.f, 0.f, 0.f};
};
} // namespace Component