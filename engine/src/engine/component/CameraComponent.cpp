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
    , mZoom(1.0f)
    , mOrthographicSize(600.0f) // Default orthographic height in world units
    , mAspectRatio(16.0f / 9.0f)
    , mNearFarZ(0.1f, 1000.0f)
    , mSmoothingEnabled(false)
    , mSmoothingFactor(5.0f)
    , mWorldMin(-1000.0f, -1000.0f)
    , mWorldMax(1000.0f, 1000.0f) {
}

void CCameraComponent::SetZoom(float z) {
    mZoom = std::min(z, std::numeric_limits<float>::max());
    setDirty(true);
}

float CCameraComponent::GetZoom() const {
    return mZoom;
}

void CCameraComponent::ZoomBy(float factor) {
    SetZoom(mZoom * factor);
}

void CCameraComponent::SetOrthographicSize(float size) {
    mOrthographicSize = std::min(size, std::numeric_limits<float>::max());
    setDirty(true);
}

float CCameraComponent::GetOrthographicSize() const {
    return mOrthographicSize;
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

const glm::vec3& CCameraComponent::GetShakeOffset() const {
    return mShakeOffset;
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

void CCameraComponent::SetAspectRatio(float aspect) {
    mAspectRatio = aspect;
    setDirty(true);
}

float CCameraComponent::GetAspectRatio() const {
    return mAspectRatio;
}

} // namespace Component