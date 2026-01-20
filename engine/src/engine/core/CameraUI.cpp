#include "engine/core/CameraUI.h"

#include <glm/gtc/matrix_transform.hpp>

#include "engine/component/CameraComponent.h"
#include "engine/utils/Logger.h"

namespace {
constexpr const char* kUICameraName = "UI Canvas";
}

namespace Core {
CCameraUI::CCameraUI() : CCamera(kUICameraName) {
}

void CCameraUI::SetOrthographicSize(float size) {
    mOrthographicSize = std::min(size, std::numeric_limits<float>::max());
    mIsDirty = true;
}

float CCameraUI::GetOrthographicSize() const {
    return mOrthographicSize;
}

void CCameraUI::EnsureUpToDate() {
    if (!mIsDirty) {
        return;
    }

    auto position = glm::vec3(0.0f, 0.0f, 0.1f);

    mProjMatrix = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, 0.0f, 1.0f);

    mViewMatrix = glm::translate(glm::mat4(1.0f), position);
    mViewProjMatrix = mProjMatrix * mViewMatrix;

    mIsDirty = false;
}
} // namespace Core
