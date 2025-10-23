#include "debug/SceneLoaderOverlord.h"

#include <engine/debug/Overlord.h>
#include <imgui.h>
#include <magic_enum/magic_enum.hpp>

#include <engine/scene/SceneHandler.h>

#include "scene/DebugScene.h"
#include "scene/SceneEnum.h"

namespace {
constexpr const char* kSceneText = "Scenes";
constexpr const char* kLoadingText = "Load scene";
} // namespace

namespace Debug {

CSceneLoaderOverlord::CSceneLoaderOverlord(Scene::CSceneHandler& sceneLoader)
    : mSceneLoader(sceneLoader) {
    SetVisible(true);
}

void CSceneLoaderOverlord::Render() {
    if (ImGui::BeginMenu(kSceneText)) {
        if (ImGui::BeginMenu(kLoadingText)) {
            for (auto& scene : magic_enum::enum_values<Scene::ESceneType>()) {
                if (ImGui::MenuItem(magic_enum::enum_name(scene).data())) {
                    mSceneLoader.CreateAndSetScene<Scene::DebugScene>();
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