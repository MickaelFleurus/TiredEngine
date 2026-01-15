#include "engine/core/Camera3D.h"

#include "engine/component/CameraComponent.h"
#include "engine/component/ComponentManager.h"
#include "engine/component/TransformComponent.h"
#include "engine/core/GameObjectBuilder.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

namespace {} // namespace

namespace Core {
CCamera3D::CCamera3D()
    : CCamera("Camera3D"), mSmoothingEnabled(false), mSmoothingFactor(5.0f) {
}

void CCamera3D::SetTarget(const glm::vec2& t) {
    mTarget = t;
    mSmoothingEnabled = true;
    mIsDirty = true;
}

void CCamera3D::ClearTarget() {
    mSmoothingEnabled = false;
    mIsDirty = true;
}

void CCamera3D::SetSmoothingFactor(float factor) {
    mSmoothingFactor = std::max(0.0001f, factor);
    mIsDirty = true;
}

void CCamera3D::SnapToTarget() {
    // if (mSmoothingEnabled) {
    //     position = target;
    mIsDirty = true;
    // }
}

void CCamera3D::SetShakeOffset(const glm::vec3& shake) {
    mShakeOffset = shake;
    mIsDirty = true;
}

const glm::vec3& CCamera3D::GetShakeOffset() const {
    return mShakeOffset;
}

void CCamera3D::SetFOV(float fovDegrees) {
    mFOV = fovDegrees;
    mIsDirty = true;
}

float CCamera3D::GetFOV() const {
    return mFOV;
}

void CCamera3D::EnsureUpToDate() {
    if (!mTransformComponent.has_value()) {
        return;
    }

    Component::CTransformComponent& transform = mTransformComponent->get();

    if (!transform.IsDirty() && !mIsDirty) {
        return;
    }

    auto position = transform.GetPosition();
    auto rotation = transform.GetRotation();

    mProjMatrix = glm::perspective(glm::radians(mFOV), mAspectRatio,
                                   mNearFarZ.x, mNearFarZ.y);

    // Create view matrix from camera position and rotation
    // Using Euler angles: rotation around X (pitch), Y (yaw), Z (roll)
    glm::mat4 rotMatrix =
        glm::eulerAngleYXZ(glm::radians(rotation.y), // yaw (Y axis)
                           glm::radians(rotation.x), // pitch (X axis)
                           glm::radians(rotation.z)  // roll (Z axis)
        );

    glm::mat4 transMatrix = glm::translate(glm::mat4(1.0f), position);

    // View matrix is the inverse of the camera's transform
    glm::mat4 cameraTransform = transMatrix * rotMatrix;
    mViewMatrix = glm::inverse(cameraTransform);

    mViewProjMatrix = mProjMatrix * mViewMatrix;

    transform.SetDirty(false);
    mIsDirty = false;
}

} // namespace Core
