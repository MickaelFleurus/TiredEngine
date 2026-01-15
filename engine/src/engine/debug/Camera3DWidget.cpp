#include "engine/debug/Camera3DWidget.h"

#include <imgui.h>

#include "engine/core/Camera3D.h"
#include "engine/core/CameraManager.h"

namespace Debug {

CCamera3DWidget::CCamera3DWidget(Core::CCameraManager& cameraManager)
    : IOverlordItem(), mCamera(cameraManager.GetDefaultCamera3D()) {
}

void CCamera3DWidget::Render() {
    if (ImGui::Begin("Camera3D Parameters", &mVisible)) {
        // FOV Control
        float fov = mCamera.GetFOV();
        if (ImGui::DragFloat("FOV (degrees)", &fov, 0.5f, 10.0f, 120.0f)) {
            mCamera.SetFOV(fov);
        }

        ImGui::Separator();

        float zoom = mCamera.GetZoom();
        if (ImGui::DragFloat("Zoom", &zoom, 0.01f, 0.1f, 10.0f)) {
            mCamera.SetZoom(zoom);
        }

        ImGui::Separator();

        glm::vec2 nearFarPlanes = mCamera.GetClipPlanes();
        if (ImGui::DragFloat2("Near/Far Planes", &nearFarPlanes.x, 1.0f, 0.0f,
                              1000.0f)) {
            mCamera.SetClipPlanes(nearFarPlanes.x, nearFarPlanes.y);
        }

        ImGui::Separator();

        float aspectRatio = mCamera.GetAspectRatio();
        if (ImGui::DragFloat("Aspect Ratio", &aspectRatio, 0.01f, 0.3f, 5.0f)) {
            mCamera.SetAspectRatio(aspectRatio);
        }

        ImGui::Separator();

        glm::vec3 shake = mCamera.GetShakeOffset();
        if (ImGui::DragFloat3("Shake Offset", &shake.x, 0.01f, -5.0f, 5.0f)) {
            mCamera.SetShakeOffset(shake);
        }

        ImGui::Separator();

        if (ImGui::CollapsingHeader("Target Tracking")) {
            // static glm::vec2 target{0.0f, 0.0f};
            // if (ImGui::DragFloat2("Target Position", &target.x, 0.1f)) {
            //     mCamera.SetTarget(target);
            // }

            // float smoothing = mCamera.GetSmoothingFactor();
            // if (ImGui::DragFloat("Smoothing Factor", &smoothing, 0.1f,
            // 0.0001f,
            //                      20.0f)) {
            //     mCamera.SetSmoothingFactor(smoothing);
            // }

            // if (ImGui::Button("Clear Target")) {
            //     mCamera.ClearTarget();
            // }
            // ImGui::SameLine();
            // if (ImGui::Button("Snap To Target")) {
            //     mCamera.SnapToTarget();
            // }
        }
        ImGui::End();
    }
}

const char* CCamera3DWidget::GetName() const {
    return "Camera3D Parameters";
}
} // namespace Debug