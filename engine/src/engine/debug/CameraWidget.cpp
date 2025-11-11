#include "engine/debug/CameraWidget.h"
#include "engine/component/CameraComponent.h"

#include <imgui.h>

namespace Debug {

CCameraWidget::CCameraWidget(Component::CCameraComponent& cameraComponent)
    : mCameraComponent(cameraComponent) {
}

void CCameraWidget::Render() {

    float zoom = mCameraComponent.GetZoom();
    if (ImGui::DragFloat("Zoom", &zoom, 0.01f, 0.1f, 10.0f)) {
        mCameraComponent.SetZoom(zoom);
    }
    float orthoSize = mCameraComponent.GetOrthographicSize();
    if (ImGui::DragFloat("Orthographic Size", &orthoSize, 1.0f, 10.0f,
                         2000.0f)) {
        mCameraComponent.SetOrthographicSize(orthoSize);
    }
    float aspectRatio = mCameraComponent.GetAspectRatio();
    if (ImGui::DragFloat("Aspect Ratio", &aspectRatio, 0.001f, 0.001f, 3.0f)) {
        mCameraComponent.SetAspectRatio(aspectRatio);
    }

    glm::vec2 nearFar = mCameraComponent.GetClipPlanes();
    if (ImGui::DragFloat2("Near/Far Planes", &nearFar.x, 0.1f, 0.1f, 1000.0f)) {
        mCameraComponent.SetClipPlanes(nearFar.x, nearFar.y);
    }
}

const char* CCameraWidget::GetName() const {
    return "Camera";
}
} // namespace Debug