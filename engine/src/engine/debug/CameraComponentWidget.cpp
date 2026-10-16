#include "engine/debug/CameraComponentWidget.h"

#include <imgui.h>

namespace Debug {
CCameraComponentWidget::CCameraComponentWidget() {
}

void CCameraComponentWidget::Render() {
    ImGui::Text("The 3D camera is using this entity.");
}

const char* CCameraComponentWidget::GetName() const {
    return "Camera Component";
}
} // namespace Debug