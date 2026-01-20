#include "engine/debug/TransformComponentWidget.h"

#include <glm/gtc/constants.hpp>
#include <imgui.h>
#include <magic_enum/magic_enum.hpp>

#include "engine/renderer/TransformHandle.h"
#include "engine/utils/Anchors.h"

namespace Debug {
CTransformWidget::CTransformWidget(Renderer::CTransformHandle transform)
    : mTransform(transform) {
    SetVisible(true);
}

void CTransformWidget::Render() {

    auto pos = mTransform.GetPosition();
    if (ImGui::DragFloat3("Position", &pos.x, 0.1f, -FLT_MAX, FLT_MAX,
                          "%.3f")) {
        mTransform.SetPosition(pos);
    }

    auto rot = mTransform.GetRotation();
    if (ImGui::DragFloat3("Rotation", &rot.x, 1.0f, -360.0f, 360.0f, "%.1f°")) {
        mTransform.SetRotation(rot);
    }

    auto scale = mTransform.GetScale();
    if (ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.0f, FLT_MAX, "%.3f")) {
        mTransform.SetScale(scale);
    }
    // TODO
    // int currentAnchor = static_cast<int>(mTransform.GetAnchor());
    // auto anchorNames = magic_enum::enum_names<Utils::EAnchors>();

    // if (ImGui::BeginCombo("Anchor", anchorNames[currentAnchor].data())) {
    //     for (int n = 0; n < static_cast<int>(anchorNames.size()); n++) {
    //         const bool isSelected = (currentAnchor == n);
    //         if (ImGui::Selectable(anchorNames[n].data(), isSelected)) {
    //             mTransform.SetAnchor(static_cast<Utils::EAnchors>(n));
    //         }
    //         if (isSelected) {
    //             ImGui::SetItemDefaultFocus();
    //         }
    //     }
    //     ImGui::EndCombo();
    // }
}

const char* CTransformWidget::GetName() const {
    return "Transform";
}
} // namespace Debug
