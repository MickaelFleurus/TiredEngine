#include "debug/SceneLoaderOverlord.h"

#include <engine/debug/Overlord.h>
#include <imgui.h>
#include <magic_enum/magic_enum.hpp>

#include "scene/ISceneLoader.h"

namespace {
constexpr const char* kSceneText = "Scenes";
constexpr const char* kLoadingText = "Load scene";
} // namespace

namespace Debug {

CSceneLoaderOverlord::CSceneLoaderOverlord(Scene::ISceneLoader& sceneLoader)
    : mSceneLoader(sceneLoader) {
    SetVisible(true);
}

void CSceneLoaderOverlord::Render() {
    if (ImGui::BeginMenu(kSceneText)) {
        if (ImGui::BeginMenu(kLoadingText)) {
            for (auto& scene : magic_enum::enum_values<Scene::ESceneType>()) {
                if (ImGui::MenuItem(magic_enum::enum_name(scene).data())) {
                    mSceneLoader.LoadScene(scene);
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
}

const char* CSceneLoaderOverlord::GetName() const {
    return "SceneLoaderOverlord";
}
} // namespace Debug