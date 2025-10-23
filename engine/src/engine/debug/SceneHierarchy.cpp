#include "engine/debug/SceneHierarchy.h"
#include "engine/core/EngineLoop.h"
#include "engine/scene/AbstractScene.h"

#include <imgui.h>

namespace {
void DrawNodeRecursive(const Core::CGameObject& obj) {

    if (ImGui::TreeNode(obj.getName().c_str())) {
        for (const auto& child : obj.getChildren()) {
            DrawNodeRecursive(*child);
        }
        ImGui::TreePop();
    }
}
} // namespace

namespace Debug {

CSceneHierarchy::CSceneHierarchy(Scene::CSceneHandler& sceneHandler)
    : mSceneHandler(sceneHandler) {
}

void CSceneHierarchy::Render() {
    Scene::CAbstractScene* currentScene = mSceneHandler.GetCurrentScene();
    if (currentScene) {
        ImGui::Text("Current Scene: %s", currentScene->GetName());
        DrawNodeRecursive(currentScene->GetRoot());
    }
}

const char* CSceneHierarchy::GetName() const {
    return "SceneHierarchy";
}
} // namespace Debug
