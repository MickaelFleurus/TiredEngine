// #include "engine/debug/SceneHierarchy.h"

// #include <imgui.h>
// #include <magic_enum/magic_enum.hpp>

// #include "engine/component/ComponentManager.h"
// #include "engine/core/EngineLoop.h"
// #include "engine/core/GameObject.h"
// #include "engine/core/GameObjectManager.h"
// #include "engine/debug/EntityWidget.h"
// #include "engine/renderer/TransformManager.h"
// #include "engine/scene/AbstractScene.h"
// #include "engine/scene/ISceneHandler.h"

// namespace {

// constexpr int kBufferSize = 256;
// static char kDefaultNameBuffer[kBufferSize]{};
// static bool isOpen{false};
// void FillDefaultNameBuffer(const std::string& name) {
//     std::copy(name.begin(), name.end(), kDefaultNameBuffer);
//     kDefaultNameBuffer[name.size()] = '\0';
// }

// } // namespace

// namespace Debug {

// CSceneHierarchy::CSceneHierarchy(Component::CComponentManager&
// componentManager,
//                                  Debug::CEntityWidget& entityWidget,
//                                  Scene::ISceneHandler& sceneHandler,
//                                  Core::CGameObjectManager& gameObjectManager)
//     : mComponentManager(componentManager)
//     , mEntityWidget(entityWidget)
//     , mSceneHandler(sceneHandler)
//     , mGameObjectManager(gameObjectManager)
//     , mTransformManager(gameObjectManager.GetTransformManager()) {
//     SetVisible(mSceneHandler.GetCurrentScene());
// }

// CSceneHierarchy::~CSceneHierarchy() = default;

// void CSceneHierarchy::Render() {
//     if (mTransformManager.HasRoots() &&
//         ImGui::Begin("Scene Hierarchy", &mVisible)) {
//         const auto& roots = mTransformManager.GetRoots();
//         ImGui::Text("Current Scene: %s",
//                     mSceneHandler.GetCurrentScene()->GetName());
//         ImGui::Separator();

//         if (ImGui::IsWindowHovered() &&
//             ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
//             mSelectedItem = std::nullopt;
//             ImGui::OpenPopup("ItemContext",
//             ImGuiPopupFlags_MouseButtonRight);
//         }
//         std::string name = "Root";
//         ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
//                                    ImGuiTreeNodeFlags_OpenOnDoubleClick;
//         if (ImGui::TreeNodeEx(name.c_str(), flags)) {
//             for (const auto& child : roots) {
//                 DrawNodeRecursive(child);
//             }
//             DrawContextMenu();
//             if (!isOpen && mModalName) {
//                 ImGui::OpenPopup(mModalName->c_str());
//             }
//             DrawNameModal();
//             ImGui::TreePop();
//         }
//         mEntityWidget.Render();

//         ImGui::End();
//     }
// }

// void CSceneHierarchy::DrawNodeRecursive(Core::GameObjectId obj) {

//     auto handle = mTransformManager.CreateHandle(obj);
//     ImGuiTreeNodeFlags flags =
//         ImGuiTreeNodeFlags_OpenOnArrow |
//         ImGuiTreeNodeFlags_OpenOnDoubleClick;

//     if (mEntityWidget.IsSelected(obj)) {
//         flags |= ImGuiTreeNodeFlags_Selected;
//     }

//     if (!handle.HasChildren()) {
//         flags |= ImGuiTreeNodeFlags_Leaf |
//         ImGuiTreeNodeFlags_NoTreePushOnOpen;
//     }
//     std::string name = mGameObjectManager.GetStringId(obj).GetName();
//     bool nodeOpen = ImGui::TreeNodeEx(name.c_str(), flags);

//     if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
//         mEntityWidget.OnItemClicked(obj);
//     }

//     if (ImGui::IsItemHovered() &&
//         ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
//         mSelectedItem = obj;
//         FillDefaultNameBuffer(name);
//         mModalName = "Rename Entity";
//         ImGui::OpenPopup(mModalName->c_str());
//     }

//     if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
//         mSelectedItem = obj;
//         ImGui::OpenPopup("ItemContext", ImGuiPopupFlags_MouseButtonRight);
//     }

//     // Draw children only if node is open AND has children
//     if (nodeOpen && handle.HasChildren()) {
//         for (const auto& child : handle.GetChildren()) {
//             DrawNodeRecursive(child);
//         }
//         ImGui::TreePop();
//     }
// }

// void CSceneHierarchy::DrawContextMenu() {

//     if (ImGui::BeginPopupContextWindow("ItemContext",
//                                        ImGuiPopupFlags_MouseButtonRight)) {
//         if (mSelectedItem) {
//             std::string name =
//                 mGameObjectManager.GetStringId(*mSelectedItem).GetName();
//             if (ImGui::MenuItem("Rename")) {
//                 FillDefaultNameBuffer(name);
//                 mModalName = "Rename Entity";
//                 ImGui::CloseCurrentPopup();
//                 ImGui::OpenPopup(mModalName->c_str());
//             }
//             // TODO
//             // if (ImGui::MenuItem("Delete")) {
//             //     mSelectedItem->DestroySelf();
//             //     mSelectedItem = nullptr;
//             //     mEntityWidget.OnItemClicked(nullptr);
//             // }
//             // if (ImGui::MenuItem("Duplicate")) {
//             //     mSelectedItem->Clone(mSelectedItem->GetParent());
//             // }
//             if (ImGui::BeginMenu("Add Component##submenu")) {
//                 for (auto type :
//                      magic_enum::enum_values<Component::EComponentType>()) {
//                     bool hasComponent =
//                         mComponentManager.HasComponent(*mSelectedItem, type);
//                     if (!hasComponent) {
//                         if (ImGui::MenuItem(
//                                 magic_enum::enum_name(type).data())) {
//                             mComponentManager.AddComponent(type,
//                                                            *mSelectedItem);
//                             mEntityWidget.OnItemClicked(mSelectedItem);
//                         }
//                     }
//                 }
//                 ImGui::EndMenu();
//             }
//         }
//         if (ImGui::MenuItem("Add Entity")) {
//             if (mSelectedItem) {
//             }
//         }

//         ImGui::EndPopup();
//     }
// }

// void CSceneHierarchy::DrawNameModal() {
//     if (!mModalName) {
//         return;
//     }
//     if (ImGui::BeginPopupModal(mModalName->c_str(), nullptr,
//                                ImGuiWindowFlags_AlwaysAutoResize)) {
//         ImGui::Text("Enter new name:");

//         // Initialize buffer only once when dialog opens
//         if (!isOpen) {
//             isOpen = true;
//             ImGui::SetKeyboardFocusHere(0);
//         }

//         bool renamed = false;
//         if (ImGui::InputText("##name_input", kDefaultNameBuffer, kBufferSize,
//                              ImGuiInputTextFlags_EnterReturnsTrue)) {
//             renamed = true;
//         }

//         if (ImGui::Button("OK", ImVec2(120, 0))) {
//             renamed = true;
//         }
//         ImGui::SameLine();
//         if (ImGui::Button("Cancel", ImVec2(120, 0))) {
//             isOpen = false;
//             mModalName = std::nullopt;
//             ImGui::CloseCurrentPopup();
//         }

//         if (renamed && mSelectedItem) {
//             mGameObjectManager.SetStringId(*mSelectedItem,
//                                            CStringId(kDefaultNameBuffer));
//             isOpen = false;
//             mModalName = std::nullopt;
//             ImGui::CloseCurrentPopup();
//         }

//         ImGui::EndPopup();
//     }
// }

// const char* CSceneHierarchy::GetName() const {
//     return "SceneHierarchy";
// }
// } // namespace Debug
