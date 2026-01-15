#include "engine/debug/SceneHierarchy.h"

#include <imgui.h>

#include "engine/core/EngineLoop.h"
#include "engine/core/GameObject.h"
#include "engine/debug/EntityWidget.h"
#include "engine/scene/AbstractScene.h"
#include "engine/scene/ISceneHandler.h"

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
        DrawContextMenu();
        mEntityWidget.Render();
    }
}

void CSceneHierarchy::DrawNodeRecursive(Core::CGameObject& obj) {

    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    // Highlight if this is the selected entity
    if (mEntityWidget.IsSelected(obj.GetId())) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    // Add leaf flag if no children
    if (obj.GetChildren().empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    bool nodeOpen = ImGui::TreeNodeEx(obj.GetName().c_str(), flags);

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        mEntityWidget.OnItemClicked(&obj);
    }

    if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
        mSelectedItem = &obj;
        ImGui::OpenPopup("ItemContext", ImGuiPopupFlags_MouseButtonRight);
    }

    // Draw children only if node is open AND has children
    if (nodeOpen && !obj.GetChildren().empty()) {
        for (const auto& child : obj.GetChildren()) {
            DrawNodeRecursive(*child);
        }
        ImGui::TreePop();
    }
}

void CSceneHierarchy::DrawContextMenu() {
    if (ImGui::BeginPopupContextWindow("ItemContext",
                                       ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::MenuItem("Delete")) {
            if (mSelectedItem.has_value()) {
                mSelectedItem.value()->DestroySelf();
                mSelectedItem = std::nullopt;
            }
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Remove this entity from the scene");
        }

        if (ImGui::MenuItem("Duplicate")) {
            if (mSelectedItem.has_value()) {
                mSelectedItem.value()->Clone(
                    mSelectedItem.value()->GetParent());
            }
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Create a copy of this entity");
        }
        if (ImGui::BeginMenu("Add Component##submenu")) {

            if (ImGui::MenuItem("Sprite")) {
                // Add sprite component
            }
            if (ImGui::MenuItem("Text")) {
                // Add text component
            }
            if (ImGui::MenuItem("Text UI")) {
                // Add sprite component
            }
            if (ImGui::MenuItem("Mesh")) {
                // Add sprite component
            }
            ImGui::EndMenu();
        }

        ImGui::EndPopup();
    }
}

const char* CSceneHierarchy::GetName() const {
    return "SceneHierarchy";
}
} // namespace Debug
