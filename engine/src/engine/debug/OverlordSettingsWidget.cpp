#include "engine/debug/OverlordSettingsWidget.h"

#include "engine/scene/ISceneHandler.h"
#include "engine/utils/FileHandler.h"

#include <imgui.h>

namespace Debug {
COverlordSettingsWidget::COverlordSettingsWidget(
    Utils::CFileHandler& fileHandler, Scene::ISceneHandler& sceneHandler)
    : mFileHandler(fileHandler), mSceneHandler(sceneHandler) {
}

void COverlordSettingsWidget::Render() {
    if (ImGui::Begin("Overlord Settings")) {
        const auto& sceneNames = mSceneHandler.GetSceneNames();

        ImGui::Text("Default scene to load:");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##scenes", sceneNames[0].c_str())) {
            for (int n = 0; n < static_cast<int>(sceneNames.size()); n++) {
                const bool isSelected = (0 == n);
                if (ImGui::Selectable(sceneNames[n].c_str(), isSelected)) {
                    mDefaultSceneToLoad = sceneNames[n];
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::End();
        if (ImGui::Button("Save")) {
        }
    }
}

const char* COverlordSettingsWidget::GetName() const {
    return "Overlord Settings";
}

} // namespace Debug