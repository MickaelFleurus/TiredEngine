#include "engine/debug/SceneLoaderOverlord.h"

#include <imgui.h>
#include <magic_enum/magic_enum.hpp>

#include "engine/scene/ISceneHandler.h"

namespace {
constexpr const char* kSceneText = "Scenes";
constexpr const char* kLoadingText = "Load scene";
} // namespace

namespace Debug {

CSceneLoaderOverlord::CSceneLoaderOverlord(Scene::ISceneHandler& sceneHandler)
    : mSceneHandler(sceneHandler) {
    SetVisible(true);
}

void CSceneLoaderOverlord::Render() {
    if (ImGui::BeginMenu(kSceneText)) {
        if (ImGui::BeginMenu(kLoadingText)) {
            for (auto& scene : mSceneHandler.GetSceneNames()) {
                if (ImGui::MenuItem(scene.c_str())) {
                    mSceneHandler.CreateAndSetScene(scene);
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