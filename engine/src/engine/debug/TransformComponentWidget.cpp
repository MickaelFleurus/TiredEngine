#include "engine/debug/TransformComponentWidget.h"

#include "engine/component/TransformComponent.h"

#include <glm/gtc/constants.hpp>
#include <imgui.h>

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

    auto rotEuler = mTransformComponent.getRotation();
    glm::vec3 rot = glm::degrees(rotEuler);
    if (ImGui::DragFloat3("Rotation", &rot.x, 1.0f, -360.0f, 360.0f, "%.1f°")) {
        mTransformComponent.setRotation(glm::radians(rot));
    }

    auto scale = mTransformComponent.getScale();
    if (ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.0f, FLT_MAX, "%.3f")) {
        mTransformComponent.setScale(scale);
    }
}

const char* CTransformComponentWidget::GetName() const {
    return "Transform";
}
} // namespace Debug