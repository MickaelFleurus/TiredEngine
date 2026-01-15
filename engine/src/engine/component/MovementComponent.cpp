#include "engine/component/MovementComponent.h"

#include <algorithm>

#include <glm/glm.hpp>

#include "engine/component/ComponentManager.h"
#include "engine/component/SpriteComponent.h"
#include "engine/core/GameObject.h"
#include "engine/utils/Math.h"

namespace {
void UpdatePosition(Core::CGameObject& current,
                    Component::CComponentManager& componentManager) {
    auto spriteComponent =
        componentManager.GetComponent<Component::CSpriteComponent>(
            current.GetId());

    for (const auto& child : current.GetChildren()) {
        UpdatePosition(*child, componentManager);
    }
}
} // namespace

namespace Component {
CMovementComponent::CMovementComponent(Core::CGameObject& owner,
                                       CComponentManager& componentManager,
                                       float acceleration)
    : IComponent(owner, componentManager, Core::EDirtyType::None)
    , mAcceleration(acceleration) {
}

CMovementComponent::CMovementComponent(Core::CGameObject& owner,
                                       CComponentManager& componentManager,
                                       const CMovementComponent& other)
    : CMovementComponent(owner, componentManager, other.mAcceleration) {
    *this = other;
}

CMovementComponent&
CMovementComponent::operator=(const CMovementComponent& other) {
    mAcceleration = other.mAcceleration;
    mVelocity = {};
    mDirection = other.mDirection;
    mIsDirty = true;
    return *this;
}

const glm::vec3& CMovementComponent::GetVelocity() const {
    return mVelocity;
}

void CMovementComponent::SetDirection(glm::vec3 direction) {
    mDirection = glm::normalize(direction);
}

const glm::vec3& CMovementComponent::GetDirection() const {
    return mDirection;
}

void CMovementComponent::Update(float deltaTime) {
    if (Utils::IsZero(mDirection)) {
        return;
    }
    mVelocity = mDirection * mAcceleration * deltaTime;

    auto position = mOwner.GetLocalPosition();
    position += mVelocity;
    ApplyPosition(position);
}

void CMovementComponent::AddDirection(glm::vec3 direction) {
    SetDirection(mDirection + direction);
}

void CMovementComponent::ApplyPosition(const glm::vec3& position) {
    // mOwner.SetLocalPosition(position);
    UpdatePosition(mOwner, mComponentManager);
}

} // namespace Component
