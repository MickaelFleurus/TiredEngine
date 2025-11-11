#include "engine/core/UICamera.h"

#include "engine/component/CameraComponent.h"
#include "engine/component/TransformComponent.h"
#include <glm/gtc/matrix_transform.hpp>

namespace {
constexpr const char* kUICameraName = "UICamera";
}

namespace Core {
CUICamera::CUICamera(CGameObject& parent, CGameObjectBuilder& builder,
                     Component::CComponentManager& componentManager)
    : CCamera(parent, builder, componentManager, kUICameraName) {
    mCameraComponent.SetOrthographicSize(1080.0f);
    mCameraComponent.SetAspectRatio(16.0f / 9.0f);
    mCameraComponent.SetZoom(1.0f);
    mTransformComponent.setPosition({0.0f, 0.0f, 0.0f});
}

void CUICamera::EnsureUpToDate() {
    if (!mCameraComponent.isDirty() && !mTransformComponent.isDirty())
        return;

    auto position = mTransformComponent.getPosition();

    // Create orthographic projection that maps (0,0) to (1920, 1080)
    // This gives us a 1:1 pixel mapping for UI
    auto& nearFarZ = mCameraComponent.GetClipPlanes();
    mProjMatrix =
        glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, nearFarZ.x, nearFarZ.y);

    // Simple view matrix - just translate by camera position
    mViewMatrix = glm::translate(glm::mat4(1.0f), -position);
    mViewProjMatrix = mProjMatrix * mViewMatrix;

    mCameraComponent.setDirty(false);
    mTransformComponent.setDirty(false);
}
} // namespace Core