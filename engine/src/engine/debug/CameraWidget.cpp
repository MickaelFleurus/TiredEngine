#include "engine/debug/CameraWidget.h"
#include "engine/component/CameraComponent.h"

#include <imgui.h>

namespace Debug {

CCameraWidget::CCameraWidget(Component::CCameraComponent& cameraComponent)
    : mCameraComponent(cameraComponent) {
}

void CCameraWidget::Render() {

    float zoom = mCameraComponent.GetZoom();
    if (ImGui::DragFloat("Zoom", &zoom, 0.1f, 0.1f, 100.0f)) {
        mCameraComponent.SetZoom(zoom);
    }
    glm::vec2 viewportSize = mCameraComponent.GetViewportSize();
    if (ImGui::DragFloat2("Viewport Size", &viewportSize.x, 1.0f, 1.0f,
                          4000.0f)) {
        mCameraComponent.SetViewportSize(viewportSize);
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