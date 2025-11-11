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
    mIsDirty = true;
}

const glm::vec3& CTransformComponent::getPosition() const {
    return mPosition;
}

void CTransformComponent::setRotation(const glm::vec3& rotation) {
    mRotation = glm::quat(rotation);
    mIsDirty = true;
}

glm::vec3 CTransformComponent::getRotation() const {
    return glm::eulerAngles(mRotation);
}

void CTransformComponent::setScale(const glm::vec3& scale) {
    mScale = scale;
    mIsDirty = true;
}

const glm::vec3& CTransformComponent::getScale() const {
    return mScale;
}

void CTransformComponent::setAnchor(Utils::EAnchors anchor) {
    mAnchor = anchor;
    mIsDirty = true;
}

Utils::EAnchors CTransformComponent::getAnchor() const {
    return mAnchor;
}

void CTransformComponent::UpdateMatrix(glm::mat4& parentTransform,
                                       const glm::vec2& size) const {
    glm::vec2 anchorOffset = Utils::GetAnchorOffset(mAnchor);
    glm::vec3 adjustedPosition =
        mPosition - glm::vec3(anchorOffset * size, 0.0f);
    parentTransform = glm::translate(parentTransform, adjustedPosition);
    parentTransform *= glm::mat4_cast(mRotation);
    parentTransform = glm::scale(parentTransform, mScale);
}
} // namespace Component