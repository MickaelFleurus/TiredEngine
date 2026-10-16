#include "engine/core/CameraUI.h"

#include <glm/gtc/matrix_transform.hpp>

#include "engine/utils/Logger.h"

namespace {
constexpr const char* kUICameraName = "UI Canvas";
}

namespace Core {
CCameraUI::CCameraUI() : CCamera(kUICameraName) {
}

void CCameraUI::SetOrthographicSize(float size) {
    mOrthographicSize = std::max(size, 0.001f);
    mIsDirty = true;
}

float CCameraUI::GetOrthographicSize() const {
    return mOrthographicSize;
}

glm::vec2 CCameraUI::GetCanvasSize() const {
    const float zoom = std::max(mZoom, 0.001f);
    const float height = mOrthographicSize / zoom;
    return {height * mAspectRatio, height};
}

void CCameraUI::EnsureUpToDate() {
    if (!mIsDirty) {
        return;
    }

    auto position = glm::vec3(0.0f, 0.0f, 0.1f);

    const auto canvasSize = GetCanvasSize();
    mProjMatrix = glm::ortho(0.0f, canvasSize.x, 0.0f, canvasSize.y,
                             mNearFarZ.x, mNearFarZ.y);

    mViewMatrix = glm::translate(glm::mat4(1.0f), position);
    mViewProjMatrix = mProjMatrix * mViewMatrix;

    mIsDirty = false;
}
} // namespace Core
