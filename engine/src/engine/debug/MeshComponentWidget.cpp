#include "engine/debug/MeshComponentWidget.h"

#include "engine/component/MeshComponent.h"

#include <imgui.h>
#include <imgui_stdlib.h>

namespace Debug {
CMeshComponentWidget::CMeshComponentWidget(
    Component::CMeshComponent& meshComponent)
    : mMeshComponent(meshComponent), mColor(meshComponent.GetColor()) {
    SetVisible(true);
}

void CMeshComponentWidget::Render() {

    if (ImGui::ColorEdit4("Font Color", &mColor.r)) {
        mMeshComponent.SetColor(mColor);
    }
}

const char* CMeshComponentWidget::GetName() const {
    return "Mesh Component";
}
} // namespace Debug