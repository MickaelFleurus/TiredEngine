#include "engine/component/CameraComponent.h"
#include "engine/component/ComponentManager.h"
#include "engine/component/TransformComponent.h"
#include "engine/core/GameObject.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>

namespace Component {
CCameraComponent::CCameraComponent(Core::CGameObject& owner,
                                   CComponentManager& componentManager)
    : IComponent(owner, componentManager)
    , mTransformComponent(
          *componentManager.getComponent<CTransformComponent>(owner.getId()))
    , mZoom(1.0f)
    , mViewportSize(800.0f, 600.0f)
    , mNearFarZ(0.1f, 1000.0f)
    , mSmoothingEnabled(false)
    , mTarget(0.0f, 0.0f)
    , mSmoothingFactor(5.0f)
    , mWorldMin(-1000.0f, -1000.0f)
    , mWorldMax(1000.0f, 1000.0f) {
}

void CCameraComponent::SetZoom(float z) {
    mZoom = std::max(z, 1e-6f);
    setDirty(true);
}

float CCameraComponent::GetZoom() const {
    return mZoom;
}

void CCameraComponent::ZoomBy(float factor) {
    SetZoom(mZoom * factor);
}

void CCameraComponent::SetViewportSize(const glm::vec2& size) {
    mViewportSize = size;
    setDirty(true);
}

const glm::vec2& CCameraComponent::GetViewportSize() const {
    return mViewportSize;
}

void CCameraComponent::SetClipPlanes(float n, float f) {
    mNearFarZ.x = n;
    mNearFarZ.y = f;
    setDirty(true);
}

const glm::vec2& CCameraComponent::GetClipPlanes() const {
    return mNearFarZ;
}

void CCameraComponent::SetWorldBounds(const glm::vec2& minBound,
                                      const glm::vec2& maxBound) {
    mWorldMin = minBound;
    mWorldMax = maxBound;
    setDirty(true);
}

void CCameraComponent::SetTarget(const glm::vec2& t) {
    mTarget = t;
    mSmoothingEnabled = true;
    setDirty(true);
}

void CCameraComponent::ClearTarget() {
    mSmoothingEnabled = false;
    setDirty(true);
}

void CCameraComponent::SetSmoothingFactor(float factor) {
    mSmoothingFactor = std::max(0.0001f, factor);
}

void CCameraComponent::SnapToTarget() {
    // if (mSmoothingEnabled) {
    //     position = target;
    setDirty(true);
    // }
}

void CCameraComponent::SetShakeOffset(const glm::vec3& shake) {
    mShakeOffset = shake;
    setDirty(true);
}

const glm::mat4& CCameraComponent::GetViewMatrix() {
    EnsureUpToDate();
    return mViewMatrix;
}

const glm::mat4& CCameraComponent::GetProjectionMatrix() {
    EnsureUpToDate();
    return mProjMatrix;
}

const glm::mat4& CCameraComponent::GetViewProjection() {
    EnsureUpToDate();
    return mViewProjMatrix;
}

CCameraComponent::SMatrices CCameraComponent::GetMatrices() {
    EnsureUpToDate();
    SMatrices mats;
    mats.projection = mProjMatrix;
    mats.view = mViewMatrix;
    mats.model = glm::mat4(1.0f);
    return mats;
}

glm::vec2 CCameraComponent::ScreenToWorld(const glm::vec2& screenPos) {
    EnsureUpToDate();
    float nx = (screenPos.x / mViewportSize.x) * 2.0f - 1.0f;
    float ny = 1.0f - (screenPos.y / mViewportSize.y) * 2.0f;
    glm::vec4 ndc(nx, ny, 0.0f, 1.0f);
    glm::mat4 inv = glm::inverse(mViewProjMatrix);
    glm::vec4 world = inv * ndc;
    return glm::vec2(world.x, world.y);
}

glm::vec2 CCameraComponent::WorldToScreen(const glm::vec2& worldPos) {
    EnsureUpToDate();
    glm::vec4 clip = mViewProjMatrix * glm::vec4(worldPos, 0.0f, 1.0f);
    if (clip.w != 0.0f)
        clip /= clip.w;
    glm::vec2 ndc = glm::vec2(clip.x, clip.y);
    glm::vec2 screen;
    screen.x = (ndc.x * 0.5f + 0.5f) * mViewportSize.x;
    screen.y = (1.0f - (ndc.y * 0.5f + 0.5f)) * mViewportSize.y;
    return screen;
}

void CCameraComponent::update(float dt) {
    // if (mSmoothingEnabled) {
    //     float alpha = 1.0f - std::exp(-mSmoothingFactor * dt);
    //     position = glm::mix(position, target, alpha);
    //     mDirty = true;
    // }
    // if (mDirty)
    //     ensureUpToDate();
}

CTransformComponent& CCameraComponent::GetTransformComponent() const {
    return mTransformComponent;
}

glm::vec4 CCameraComponent::GetScissorRect() const {
    return glm::vec4(0.0f, 0.0f, mViewportSize.x, mViewportSize.y);
}

glm::vec4 CCameraComponent::GetWorldAABB() {
    EnsureUpToDate();
    glm::mat4 inv = glm::inverse(mViewProjMatrix);
    glm::vec4 cornersNDC[4] = {glm::vec4(-1, -1, 0, 1), glm::vec4(1, -1, 0, 1),
                               glm::vec4(-1, 1, 0, 1), glm::vec4(1, 1, 0, 1)};
    glm::vec2 mn(1e30f), mx(-1e30f);
    for (int i = 0; i < 4; i++) {
        glm::vec4 c = inv * cornersNDC[i];
        c /= c.w;
        mn = glm::min(mn, glm::vec2(c.x, c.y));
        mx = glm::max(mx, glm::vec2(c.x, c.y));
    }
    return glm::vec4(mn.x, mn.y, mx.x, mx.y);
}

void CCameraComponent::EnsureUpToDate() {
    if (!isDirty())
        return;

    auto position = mTransformComponent.getPosition();
    float rotationDeg = mTransformComponent.getRotation().z;

    float hw = mViewportSize.x * 0.5f;
    float hh = mViewportSize.y * 0.5f;
    float invZoom = 1.0f / mZoom;
    float left = -hw * invZoom;
    float right = hw * invZoom;
    float bottom = -hh * invZoom;
    float top = hh * invZoom;
    mProjMatrix =
        glm::ortho(left, right, bottom, top, mNearFarZ.x, mNearFarZ.y);

    glm::mat4 t = glm::translate(glm::mat4(1.0f), -position - mShakeOffset);
    glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(-rotationDeg),
                              glm::vec3(0, 0, 1));
    mViewMatrix = r * t;
    mViewProjMatrix = mProjMatrix * mViewMatrix;

    setDirty(false);
}
} // namespace Component