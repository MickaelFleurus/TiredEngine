#include "engine/debug/SceneHierarchy.h"
#include "engine/core/EngineLoop.h"
#include "engine/scene/AbstractScene.h"

#include "engine/debug/EntityWidget.h"
#include "engine/scene/ISceneHandler.h"
#include <imgui.h>

namespace {} // namespace

namespace Debug {

CSceneHierarchy::CSceneHierarchy(Debug::CEntityWidget& entityWidget,
                                 Scene::ISceneHandler& sceneHandler)
    : mEntityWidget(entityWidget), mSceneHandler(sceneHandler) {
    SetVisible(true);
}

CSceneHierarchy::~CSceneHierarchy() = default;

void CSceneHierarchy::Render() {
    Scene::CAbstractScene* currentScene = mSceneHandler.GetCurrentScene();
    if (currentScene) {
        ImGui::Text("Current Scene: %s", currentScene->GetName());
        DrawNodeRecursive(currentScene->GetRoot());
    }
}

void CSceneHierarchy::DrawNodeRecursive(const Core::CGameObject& obj) {

    bool nodeOpen = ImGui::TreeNode(obj.getName().c_str());

    // Check for clicks IMMEDIATELY after TreeNode, before drawing children
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        mEntityWidget.OnItemClicked(obj.getId());
    }

    // Right-click context menu (also needs to be right after TreeNode)
    // if (ImGui::BeginPopupContextItem()) {
    //     if (ImGui::MenuItem("Inspect")) {
    //         mEntityWidget.OnItemClicked(obj.getId());
    //     }
    //     if (ImGui::MenuItem("Delete")) {
    //         // Delete logic
    //     }
    //     ImGui::EndPopup();
    // }

    // Now draw children if node is open
    if (nodeOpen) {
        for (const auto& child : obj.getChildren()) {
            DrawNodeRecursive(*child);
        }
        ImGui::TreePop();
    }
}

const char* CSceneHierarchy::GetName() const {
    return "SceneHierarchy";
}
} // namespace Debug
