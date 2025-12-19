#include "engine/debug/LevelSceneLoaderWidget.h"

#include <imgui.h>

#include "engine/scene/SceneLoader.h"
#include "engine/utils/FileHandler.h"

namespace Debug {

CLevelSceneLoaderWidget::CLevelSceneLoaderWidget(
    Utils::CFileHandler& fileHandler, Scene::CSceneLoader& sceneLoader)
    : mFileHandler(fileHandler), mSceneLoader(sceneLoader) {
}

void CLevelSceneLoaderWidget::Render() {
    if (mNeedsRefresh) {
        mSceneFiles = mFileHandler.GetFileNames(".yaml", "/levels");
        mSelectedSceneIndex = -1;
        mNeedsRefresh = false;
    }
    bool isVisible = IsVisible();
    if (!ImGui::Begin("Level Scene Loader", &isVisible)) {
        return;
    }

    ImGui::Text("Available Scenes:");
    ImGui::Separator();

    // Display the list of scenes
    if (ImGui::BeginListBox("##scenes", ImVec2(-1, 200))) {
        for (int i = 0; i < static_cast<int>(mSceneFiles.size()); ++i) {
            const char* sceneName = mSceneFiles[i].c_str();
            bool isSelected = (mSelectedSceneIndex == i);

            if (ImGui::Selectable(sceneName, isSelected,
                                  ImGuiSelectableFlags_AllowDoubleClick)) {
                mSelectedSceneIndex = i;
                mSelectedScenePath = mFileHandler.GetAssetsFolder() +
                                     "/levels/" + mSceneFiles[i];
                // Load on double-click
                if (ImGui::IsMouseDoubleClicked(0)) {
                    mSceneLoader.LoadSceneFromFile(mSelectedScenePath);
                }
            }

            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndListBox();
    }

    ImGui::Spacing();

    // Load button
    if (ImGui::Button("Load Scene", ImVec2(120, 0))) {
        if (mSelectedSceneIndex >= 0 &&
            mSelectedSceneIndex < static_cast<int>(mSceneFiles.size())) {
            mSceneLoader.LoadSceneFromFile(mSelectedScenePath);
        }
    }

    ImGui::SameLine();

    // Refresh button
    if (ImGui::Button("Refresh", ImVec2(120, 0))) {
        mNeedsRefresh = true;
        mSelectedScenePath.clear();
        mSelectedSceneIndex = -1;
    }
    ImGui::End();
}

const char* CLevelSceneLoaderWidget::GetName() const {
    return "Level Scene Loader";
}
} // namespace Debug
