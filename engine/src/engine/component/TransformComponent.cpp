#include "engine/component/TransformComponent.h"
#include <glm/gtc/matrix_transform.hpp>

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
    mRotation = glm::quat(rotation);
}

glm::vec3 CTransformComponent::getRotation() const {
    return glm::eulerAngles(mRotation);
}

void CTransformComponent::setScale(const glm::vec3& scale) {
    mScale = scale;
}

const glm::vec3& CTransformComponent::getScale() const {
    return mScale;
}

void CTransformComponent::UpdateMatrix(glm::mat4& parentTransform) const {
    parentTransform = glm::translate(parentTransform, mPosition);
    parentTransform *= glm::mat4_cast(mRotation);
    parentTransform = glm::scale(parentTransform, mScale);
}
} // namespace Component