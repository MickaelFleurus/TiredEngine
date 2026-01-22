#include "engine/debug/TransformComponentWidget.h"

#include <glm/gtc/constants.hpp>
#include <imgui.h>
#include <magic_enum/magic_enum.hpp>

#include "engine/renderer/TransformHandle.h"
#include "engine/utils/Anchors.h"

namespace {
glm::vec3 QuatToEulerDeg(const glm::quat& q) {
    glm::vec3 eulerRad = glm::eulerAngles(glm::normalize(q));
    return glm::degrees(eulerRad);
}

glm::quat EulerDegToQuat_ZYX(const glm::vec3& eulerDeg) {
    glm::vec3 r = glm::radians(eulerDeg);

    glm::quat q = glm::angleAxis(r.z, glm::vec3(0, 0, 1)) *
                  glm::angleAxis(r.y, glm::vec3(0, 1, 0)) *
                  glm::angleAxis(r.x, glm::vec3(1, 0, 0));

    return glm::normalize(q);
}
} // namespace

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

    auto rotation = mTransform.GetRotation();
    if (!mIsInitialized) {
        mRotationAngles = QuatToEulerDeg(rotation);
        mIsInitialized = true;
    }

    glm::vec3 previousEuler = mRotationAngles;
    if (ImGui::DragFloat3("Rotation", &mRotationAngles.x, 1.0f, -360.0f, 360.0f,
                          "%.1f°")) {
        glm::vec3 deltaDeg = mRotationAngles - previousEuler;

        glm::vec3 deltaRad = glm::radians(deltaDeg);

        glm::quat delta = glm::angleAxis(deltaRad.z, glm::vec3(0, 0, 1)) *
                          glm::angleAxis(deltaRad.y, glm::vec3(0, 1, 0)) *
                          glm::angleAxis(deltaRad.x, glm::vec3(1, 0, 0));
        rotation = glm::normalize(delta * rotation);
        mTransform.SetRotation(rotation);
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
