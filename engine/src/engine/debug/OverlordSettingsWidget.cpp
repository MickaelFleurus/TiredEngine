#include "engine/debug/OverlordSettingsWidget.h"

#include "engine/scene/ISceneHandler.h"
#include "engine/utils/FileHandler.h"

#include "engine/utils/OverlordSettings.h"

#include <imgui.h>

namespace {} // namespace

namespace Debug {
COverlordSettingsWidget::COverlordSettingsWidget(
    Utils::COverlordSettings& settings, Utils::CFileHandler& fileHandler,
    Scene::ISceneHandler& sceneHandler)
    : mSettings(settings)
    , mFileHandler(fileHandler)
    , mSceneHandler(sceneHandler)
    , mSceneNames(sceneHandler.GetSceneNames())
    , mHasDefaultSceneToLoad(settings.HasDefaultSceneToLoad())
    , mSelectedDefaultScene(settings.GetDefaultSceneToLoad()) {
    SetVisible(true);
}

void COverlordSettingsWidget::Render() {
    if (ImGui::BeginMenu("Settings")) {
        mShowWidget = true;
        ImGui::EndMenu();
    }
    if (mShowWidget && ImGui::Begin("Overlord Settings")) {

        auto currentDefaultScene = mSelectedDefaultScene;

        if (ImGui::RadioButton("Has a default scene to load",
                               mHasDefaultSceneToLoad)) {
            mHasDefaultSceneToLoad = !mHasDefaultSceneToLoad;
        }
        if (mHasDefaultSceneToLoad) {
            ImGui::Text("Default scene to load:");
            ImGui::SameLine();
            if (ImGui::BeginCombo("##scenes", currentDefaultScene.c_str())) {
                for (const auto& scene : mSceneNames) {
                    const bool isSelected = (scene == currentDefaultScene);
                    if (ImGui::Selectable(scene.c_str(), isSelected)) {
                        mSelectedDefaultScene = scene;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
        }
        if (ImGui::Button("Close")) {
            mShowWidget = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            if (mHasDefaultSceneToLoad) {
                mSettings.SetDefaultSceneToLoad(mSelectedDefaultScene);
            } else {
                mSettings.SetDefaultSceneToLoad("");
            }
            mSettings.SaveSettings();
            mShowWidget = false;
        }
        ImGui::End();
    }
}

const char* COverlordSettingsWidget::GetName() const {
    return "Overlord Settings";
}

} // namespace Debug