#include "engine/component/TransformComponent.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

namespace Component {
CTransformComponent::CTransformComponent(Core::CGameObject& owner,
                                         CComponentManager& componentManager)
    : IComponent(owner, componentManager, Core::EDirtyType::Transform) {
}

void CTransformComponent::Update(float deltaTime) {
    // Update logic can be implemented here if needed
}

void CTransformComponent::SetPosition(const glm::vec3& position) {
    mPosition = position;
    SetDirty(true);
}

const glm::vec3& CTransformComponent::GetPosition() const {
    return mPosition;
}

void CTransformComponent::SetRotation(const glm::vec3& rotation) {
    mRotation = rotation;
    glm::vec3 radians = glm::radians(rotation);
    mRotationQuat =
        glm::quat(glm::eulerAngleYXZ(radians.y, radians.x, radians.z));
    SetDirty(true);
}

glm::vec3 CTransformComponent::GetRotation() const {
    return mRotation;
}

void CTransformComponent::SetScale(const glm::vec3& scale) {
    mScale = scale;
    SetDirty(true);
}

const glm::vec3& CTransformComponent::GetScale() const {
    return mScale;
}

void CTransformComponent::SetAnchor(Utils::EAnchors anchor) {
    mAnchor = anchor;
    SetDirty(true);
}

Utils::EAnchors CTransformComponent::GetAnchor() const {
    return mAnchor;
}

void CTransformComponent::UpdateMatrix(glm::mat4& parentTransform,
                                       const glm::vec2& size) const {
    glm::vec2 anchorOffset = Utils::GetAnchorOffset(mAnchor);
    glm::vec3 adjustedPosition =
        mPosition - glm::vec3(anchorOffset * size, 0.0f);
    parentTransform = glm::translate(parentTransform, adjustedPosition);
    parentTransform *= glm::mat4_cast(mRotationQuat);
    parentTransform = glm::scale(parentTransform, mScale);
}
} // namespace Component