#include "engine/debug/CameraUIWidget.h"

#include <imgui.h>

#include "engine/core/CameraManager.h"
#include "engine/core/CameraUI.h"

namespace Debug {

CCameraUIWidget::CCameraUIWidget(Core::CCameraManager& cameraManager)
    : IOverlordItem(), mCamera(cameraManager.GetCameraUI()) {
}

void CCameraUIWidget::Render() {
    if (ImGui::Begin("UI Camera Parameters", &mVisible)) {
        float zoom = mCamera.GetZoom();
        if (ImGui::DragFloat("Zoom", &zoom, 0.01f, 0.1f, 10.0f)) {
            mCamera.SetZoom(zoom);
        }

        ImGui::Separator();

        glm::vec2 nearFarPlanes = mCamera.GetClipPlanes();
        if (ImGui::DragFloat2("Near/Far Planes", &nearFarPlanes.x, 0.01f, 0.1f,
                              1000.0f)) {
            mCamera.SetClipPlanes(nearFarPlanes.x, nearFarPlanes.y);
        }

        ImGui::Separator();

        float aspectRatio = mCamera.GetAspectRatio();
        if (ImGui::DragFloat("Aspect Ratio", &aspectRatio, 0.01f, 0.3f, 5.0f)) {
            mCamera.SetAspectRatio(aspectRatio);
        }

        ImGui::Separator();

        float orth = mCamera.GetOrthographicSize();
        if (ImGui::DragFloat("Orthographic Size", &orth, 1.0f, 1.0f, 1000.0f)) {
            mCamera.SetOrthographicSize(orth);
        }

        ImGui::End();
    }
}

const char* CCameraUIWidget::GetName() const {
    return "UI Camera Parameters";
}
} // namespace Debug