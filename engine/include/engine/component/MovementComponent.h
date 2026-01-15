#pragma once
#include <glm/vec3.hpp>

#include "engine/component/IComponent.h"

namespace Component {
class CMovementComponent : public IComponent {
public:
    CMovementComponent(Core::CGameObject& owner,
                       CComponentManager& componentManager, float acceleration);
    CMovementComponent(Core::CGameObject& owner,
                       CComponentManager& componentManager,
                       const CMovementComponent& other);

    CMovementComponent& operator=(const CMovementComponent& other);
    CMovementComponent(const CMovementComponent& other) = delete;
    CMovementComponent(const CMovementComponent&& other) = delete;
    CMovementComponent& operator=(CMovementComponent&& other) = delete;

    void Update(float deltaTime) override;

    void AddDirection(glm::vec3 direction);
    void SetDirection(glm::vec3 direction);
    const glm::vec3& GetDirection() const;

    const glm::vec3& GetVelocity() const;

    void ApplyPosition(const glm::vec3& position);

private:
    float mAcceleration;

    glm::vec3 mDirection{0.f, 0.f, 0.f};
    glm::vec3 mVelocity{0.f, 0.f, 0.f};
};
} // namespace Component
