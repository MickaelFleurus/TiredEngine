#include "engine/core/3DCamera.h"

#include "engine/component/CameraComponent.h"
#include "engine/component/ComponentManager.h"
#include "engine/component/TransformComponent.h"
#include "engine/core/GameObjectBuilder.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

namespace {
Core::CGameObject& BuildEntity(Core::CGameObjectBuilder::CBuilder builder,
                               Component::CComponentManager& componentManager) {
    Core::CGameObject& entity = *builder.AddCameraComponent().Build();
    return entity;
}
} // namespace

namespace Core {
C3DCamera::C3DCamera(CGameObject& parent, CGameObjectBuilder& builder,
                     Component::CComponentManager& componentManager)
    : CCamera(parent, builder, componentManager, "Camera3D") {
}

void C3DCamera::SetFOV(float fovDegrees) {
    mFOV = fovDegrees;
    mCameraComponent.SetDirty(true);
}

float C3DCamera::GetFOV() const {
    return mFOV;
}

void C3DCamera::EnsureUpToDate() {
    if (!mCameraComponent.IsDirty() && !mTransformComponent.IsDirty())
        return;

    auto position = mTransformComponent.GetPosition();
    auto rotation = mTransformComponent.GetRotation();

    // Create perspective projection matrix
    float aspectRatio = mCameraComponent.GetAspectRatio();
    auto& nearFarZ = mCameraComponent.GetClipPlanes();

    mProjMatrix = glm::perspective(glm::radians(mFOV), aspectRatio, nearFarZ.x,
                                   nearFarZ.y);

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

    mCameraComponent.SetDirty(false);
    mTransformComponent.SetDirty(false);
}

} // namespace Core