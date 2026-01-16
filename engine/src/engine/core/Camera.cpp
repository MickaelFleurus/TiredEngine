#include "engine/core/Camera.h"

#include "engine/component/TransformComponent.h"
#include "engine/utils/Logger.h"

namespace {} // namespace

namespace Core {
CCamera::CCamera(std::string cameraName) : mName(std::move(cameraName)) {
}

void CCamera::SetTransformComponent(
    std::optional<std::reference_wrapper<Component::CTransformComponent>>
        transformComponent) {
    if (mTransformComponent.has_value()) {
        LOG_WARNING("Having two camera is not supported yet. The last "
                    "registered component will set the transform. This is most "
                    "likely going to be an issue.");
    }
    mTransformComponent = transformComponent;
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

void CCamera::SetZoom(float z) {
    mZoom = std::min(z, std::numeric_limits<float>::max());
    mIsDirty = true;
}

float CCamera::GetZoom() const {
    return mZoom;
}

void CCamera::ZoomBy(float factor) {
    SetZoom(mZoom * factor);
}

void CCamera::SetClipPlanes(float n, float f) {
    mNearFarZ.x = n;
    mNearFarZ.y = f;
    mIsDirty = true;
}

const glm::vec2& CCamera::GetClipPlanes() const {
    return mNearFarZ;
}

void CCamera::SetAspectRatio(float aspect) {
    mAspectRatio = aspect;
    mIsDirty = true;
}

float CCamera::GetAspectRatio() const {
    return mAspectRatio;
}

} // namespace Core