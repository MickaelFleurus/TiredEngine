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
        componentManager.getComponent<Component::CSpriteComponent>(
            current.getId());
    if (spriteComponent) {
        spriteComponent->updatePosition();
    }
    for (const auto& child : current.getChildren()) {
        UpdatePosition(*child, componentManager);
    }
}
} // namespace

namespace Component {
CMovementComponent::CMovementComponent(Core::CGameObject& owner,
                                       CComponentManager& componentManager,
                                       float acceleration)
    : IComponent(owner, componentManager), mAcceleration(acceleration) {
}

const glm::vec2& CMovementComponent::getVelocity() const {
    return mVelocity;
}

void CMovementComponent::setDirection(glm::vec2 direction) {
    mDirection = glm::normalize(direction);
}

const glm::vec2& CMovementComponent::getDirection() const {
    return mDirection;
}

void CMovementComponent::update(float deltaTime) {
    if (Utils::IsZero(mDirection)) {
        return;
    }
    mVelocity = mDirection * mAcceleration * deltaTime;

    auto position = mOwner.getLocalPosition();
    position += mVelocity;
    applyPosition(position);
}

void CMovementComponent::addDirection(glm::vec2 direction) {
    setDirection(mDirection + direction);
}

void CMovementComponent::applyPosition(const glm::vec2& position) {
    mOwner.setLocalPosition(position);
    UpdatePosition(mOwner, mComponentManager);
}

} // namespace Component