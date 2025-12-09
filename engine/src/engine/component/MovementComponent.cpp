#include "engine/component/MovementComponent.h"

#include "engine/component/ComponentManager.h"
#include "engine/component/SpriteComponent.h"
#include "engine/core/GameObject.h"
#include "engine/utils/Math.h"

#include <algorithm>
#include <glm/glm.hpp>

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

const glm::vec3& CMovementComponent::getVelocity() const {
    return mVelocity;
}

void CMovementComponent::setDirection(glm::vec3 direction) {
    mDirection = glm::normalize(direction);
}

const glm::vec3& CMovementComponent::getDirection() const {
    return mDirection;
}

void CMovementComponent::Update(float deltaTime) {
    if (Utils::IsZero(mDirection)) {
        return;
    }
    mVelocity = mDirection * mAcceleration * deltaTime;

    auto position = mOwner.GetLocalPosition();
    position += mVelocity;
    applyPosition(position);
}

void CMovementComponent::addDirection(glm::vec3 direction) {
    setDirection(mDirection + direction);
}

void CMovementComponent::applyPosition(const glm::vec3& position) {
    // mOwner.SetLocalPosition(position);
    UpdatePosition(mOwner, mComponentManager);
}

} // namespace Component