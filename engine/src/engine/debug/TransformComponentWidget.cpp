#include "engine/debug/TransformComponentWidget.h"

#include "engine/component/TransformComponent.h"
#include "engine/utils/Anchors.h"

#include <glm/gtc/constants.hpp>
#include <imgui.h>
#include <magic_enum/magic_enum.hpp>

namespace Debug {
CTransformComponentWidget::CTransformComponentWidget(
    Component::CTransformComponent& transformComponent)
    : mTransformComponent(transformComponent) {
    SetVisible(true);
}

void CTransformComponentWidget::Render() {

    auto pos = mTransformComponent.getPosition();
    if (ImGui::DragFloat3("Position", &pos.x, 0.1f, -FLT_MAX, FLT_MAX,
                          "%.3f")) {
        mTransformComponent.setPosition(pos);
    }

    auto rot = mTransformComponent.getRotation();
    if (ImGui::DragFloat3("Rotation", &rot.x, 1.0f, -360.0f, 360.0f, "%.1f°")) {
        mTransformComponent.setRotation(rot);
    }

    auto scale = mTransformComponent.getScale();
    if (ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.0f, FLT_MAX, "%.3f")) {
        mTransformComponent.setScale(scale);
    }
    int currentAnchor = static_cast<int>(mTransformComponent.getAnchor());
    auto anchorNames = magic_enum::enum_names<Utils::EAnchors>();

    if (ImGui::BeginCombo("Anchor", anchorNames[currentAnchor].data())) {
        for (int n = 0; n < static_cast<int>(anchorNames.size()); n++) {
            const bool isSelected = (currentAnchor == n);
            if (ImGui::Selectable(anchorNames[n].data(), isSelected)) {
                mTransformComponent.SetAnchor(static_cast<Utils::EAnchors>(n));
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

const char* CTransformComponentWidget::GetName() const {
    return "Transform";
}
} // namespace Debug