#include "engine/core/Camera3D.h"

#include "engine/component/CameraComponent.h"
#include "engine/component/ComponentManager.h"
#include "engine/core/GameObjectBuilder.h"
#include "engine/renderer/TransformManager.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

namespace {} // namespace

namespace Core {
CCamera3D::CCamera3D(Renderer::CTransformManager& transformManager)
    : CCamera("Camera3D")
    , mTransformManager(transformManager)
    , mSmoothingEnabled(false)
    , mSmoothingFactor(5.0f) {
    mTransformManager.RegisterObserver(*this, mToken);
    
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

void CCamera3D::OnDirty(Core::GameObjectId id) {
    if (mGameObjectId && mGameObjectId == id) {
        mIsDirty = true;
    }
}

void CCamera3D::SetFOV(float fovDegrees) {
    mFOV = fovDegrees;
    mIsDirty = true;
}

float CCamera3D::GetFOV() const {
    return mFOV;
}

void CCamera3D::EnsureUpToDate() {
    if (!mGameObjectId.has_value() || !mIsDirty) {
        return;
    }

    auto position = mTransformManager.GetPosition(*mGameObjectId);
    auto rotation = mTransformManager.GetRotation(*mGameObjectId);

    mProjMatrix = glm::perspective(glm::radians(mFOV), mAspectRatio,
                                   mNearFarZ.x, mNearFarZ.y);

    glm::mat4 rotMatrix = glm::mat4_cast(rotation);
    glm::mat4 transMatrix = glm::translate(glm::mat4(1.0f), position);

    // View matrix is the inverse of the camera's transform
    glm::mat4 cameraTransform = transMatrix * rotMatrix;
    mViewMatrix = glm::inverse(cameraTransform);

    mViewProjMatrix = mProjMatrix * mViewMatrix;

    mIsDirty = false;
}

} // namespace Core
