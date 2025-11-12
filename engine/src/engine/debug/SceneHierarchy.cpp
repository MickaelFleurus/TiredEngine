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
    SetVisible(mSceneHandler.GetCurrentScene());
}

CSceneHierarchy::~CSceneHierarchy() = default;

void CSceneHierarchy::Render() {
    Scene::CAbstractScene* currentScene = mSceneHandler.GetCurrentScene();
    if (currentScene) {
        ImGui::Text("Current Scene: %s", currentScene->GetName());
        DrawNodeRecursive(currentScene->GetRoot());
        mEntityWidget.Render();
    }
}

void CSceneHierarchy::DrawNodeRecursive(const Core::CGameObject& obj) {

    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    // Highlight if this is the selected entity
    if (mEntityWidget.GetEntityId() == obj.getId()) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    // Add leaf flag if no children
    if (obj.getChildren().empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    bool nodeOpen = ImGui::TreeNodeEx(obj.getName().c_str(), flags);

    // Left-click to select
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        mEntityWidget.OnItemClicked(obj.getId(), obj.getName());
    }

    // Draw children only if node is open AND has children
    if (nodeOpen && !obj.getChildren().empty()) {
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
