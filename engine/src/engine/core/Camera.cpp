#include "engine/core/Camera.h"

#include "engine/component/CameraComponent.h"
#include "engine/component/ComponentManager.h"
#include "engine/component/TransformComponent.h"
#include "engine/core/GameObjectBuilder.h"

namespace {
Core::CGameObject& BuildEntity(Core::CGameObjectBuilder::CBuilder builder,
                               Component::CComponentManager& componentManager) {
    Core::CGameObject& entity = *builder.AddCameraComponent().Build();
    return entity;
}
} // namespace

namespace Core {
CCamera::CCamera(CGameObject& parent, CGameObjectBuilder& builder,
                 Component::CComponentManager& componentManager,
                 std::string cameraName)
    : mEntity(BuildEntity(builder.CreateBuilder(cameraName, parent),
                          componentManager))
    , mCameraComponent(
          *componentManager.getComponent<Component::CCameraComponent>(
              mEntity.getId()))
    , mTransformComponent(
          *componentManager.getComponent<Component::CTransformComponent>(
              mEntity.getId())) {
}

const glm::mat4& CCamera::GetViewMatrix() {
    EnsureUpToDate();
    return mViewMatrix;
}

const glm::mat4& CCamera::GetProjectionMatrix() {
    EnsureUpToDate();
    return mProjMatrix;
}

const glm::mat4& CCamera::GetViewProjection() {
    EnsureUpToDate();
    return mViewProjMatrix;
}

CCamera::SMatrices CCamera::GetMatrices() {
    EnsureUpToDate();
    SMatrices mats;
    mats.projection = mProjMatrix;
    mats.view = mViewMatrix;
    mats.model = glm::mat4(1.0f);
    return mats;
}

void CCamera::EnsureUpToDate() {
    if (!mCameraComponent.isDirty() && !mTransformComponent.isDirty())
        return;

    auto position = mTransformComponent.getPosition();
    float rotationDeg = mTransformComponent.getRotation().z;

    float orthoHeight =
        mCameraComponent.GetOrthographicSize() / mCameraComponent.GetZoom();
    float orthoWidth = orthoHeight * mCameraComponent.GetAspectRatio();

    // Create projection matrix with world-space bounds
    float left = -orthoWidth * 0.5f;
    float right = orthoWidth * 0.5f;
    float bottom = -orthoHeight * 0.5f;
    float top = orthoHeight * 0.5f;

    auto& nearFarZ = mCameraComponent.GetClipPlanes();
    mProjMatrix = glm::ortho(left, right, bottom, top, nearFarZ.x, nearFarZ.y);

    const glm::vec3& shakeOffset = mCameraComponent.GetShakeOffset();
    glm::mat4 t = glm::translate(glm::mat4(1.0f), -position - shakeOffset);
    glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(-rotationDeg),
                              glm::vec3(0, 0, 1));
    mViewMatrix = r * t;
    mViewProjMatrix = mProjMatrix * mViewMatrix;

    mCameraComponent.setDirty(false);
    mTransformComponent.setDirty(false);
}

glm::vec2 CCamera::ScreenToWorld(const glm::vec2& screenPos,
                                 const glm::vec2& viewportSize) {
    EnsureUpToDate();
    float nx = (screenPos.x / viewportSize.x) * 2.0f - 1.0f;
    float ny = 1.0f - (screenPos.y / viewportSize.y) * 2.0f;
    glm::vec4 ndc(nx, ny, 0.0f, 1.0f);
    glm::mat4 inv = glm::inverse(mViewProjMatrix);
    glm::vec4 world = inv * ndc;
    return glm::vec2(world.x, world.y);
}

glm::vec2 CCamera::WorldToScreen(const glm::vec2& worldPos,
                                 const glm::vec2& viewportSize) {
    EnsureUpToDate();
    glm::vec4 clip = mViewProjMatrix * glm::vec4(worldPos, 0.0f, 1.0f);
    if (clip.w != 0.0f)
        clip /= clip.w;
    glm::vec2 ndc = glm::vec2(clip.x, clip.y);
    glm::vec2 screen;
    screen.x = (ndc.x * 0.5f + 0.5f) * viewportSize.x;
    screen.y = (1.0f - (ndc.y * 0.5f + 0.5f)) * viewportSize.y;
    return screen;
}

} // namespace Core