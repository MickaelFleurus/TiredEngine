#include "engine/component/CameraComponent.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>

#include "engine/component/ComponentManager.h"
#include "engine/component/TransformComponent.h"
#include "engine/core/GameObject.h"

namespace Component {
CCameraComponent::CCameraComponent(Core::CGameObject& owner,
                                   CComponentManager& componentManager)
    : IComponent(owner, componentManager, Core::EDirtyType::None)
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
    SetDirty(true);
}

float CCameraComponent::GetZoom() const {
    return mZoom;
}

void CCameraComponent::ZoomBy(float factor) {
    SetZoom(mZoom * factor);
}

void CCameraComponent::SetOrthographicSize(float size) {
    mOrthographicSize = std::min(size, std::numeric_limits<float>::max());
    SetDirty(true);
}

float CCameraComponent::GetOrthographicSize() const {
    return mOrthographicSize;
}

void CCameraComponent::SetClipPlanes(float n, float f) {
    mNearFarZ.x = n;
    mNearFarZ.y = f;
    SetDirty(true);
}

const glm::vec2& CCameraComponent::GetClipPlanes() const {
    return mNearFarZ;
}

void CCameraComponent::SetWorldBounds(const glm::vec2& minBound,
                                      const glm::vec2& maxBound) {
    mWorldMin = minBound;
    mWorldMax = maxBound;
    SetDirty(true);
}

void CCameraComponent::SetTarget(const glm::vec2& t) {
    mTarget = t;
    mSmoothingEnabled = true;
    SetDirty(true);
}

void CCameraComponent::ClearTarget() {
    mSmoothingEnabled = false;
    SetDirty(true);
}

void CCameraComponent::SetSmoothingFactor(float factor) {
    mSmoothingFactor = std::max(0.0001f, factor);
}

void CCameraComponent::SnapToTarget() {
    // if (mSmoothingEnabled) {
    //     position = target;
    SetDirty(true);
    // }
}

void CCameraComponent::SetShakeOffset(const glm::vec3& shake) {
    mShakeOffset = shake;
    SetDirty(true);
}

const glm::vec3& CCameraComponent::GetShakeOffset() const {
    return mShakeOffset;
}

void CCameraComponent::Update(float dt) {
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
    SetDirty(true);
}

float CCameraComponent::GetAspectRatio() const {
    return mAspectRatio;
}

} // namespace Component