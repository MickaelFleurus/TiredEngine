#pragma once
#include "engine/component/IComponent.h"

#include <glm/vec2.hpp>

namespace Component {
class CMovementComponent : public IComponent {
public:
    CMovementComponent(Core::CGameObject& owner,
                       CComponentManager& componentManager, float acceleration);

    void update(float deltaTime) override;

    void addDirection(glm::vec2 direction);
    void setDirection(glm::vec2 direction);
    const glm::vec2& getDirection() const;

    const glm::vec2& getVelocity() const;

    void applyPosition(const glm::vec2& position);

private:
    const float mAcceleration;

    glm::vec2 mDirection{0.f, 0.f};
    glm::vec2 mVelocity{0.f, 0.f};
};
} // namespace Component