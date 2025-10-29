#include "engine/component/TransformComponent.h"

namespace Component {
CTransformComponent::CTransformComponent(Core::CGameObject& owner,
                                         CComponentManager& componentManager)
    : IComponent(owner, componentManager) {
}

void CTransformComponent::update(float deltaTime) {
    // Update logic can be implemented here if needed
}

void CTransformComponent::setPosition(const glm::vec3& position) {
    mPosition = position;
}

const glm::vec3& CTransformComponent::getPosition() const {
    return mPosition;
}

void CTransformComponent::setRotation(const glm::vec3& rotation) {
    mRotation = rotation;
}

const glm::vec3& CTransformComponent::getRotation() const {
    return mRotation;
}

void CTransformComponent::setScale(const glm::vec3& scale) {
    mScale = scale;
}

const glm::vec3& CTransformComponent::getScale() const {
    return mScale;
}
} // namespace Component