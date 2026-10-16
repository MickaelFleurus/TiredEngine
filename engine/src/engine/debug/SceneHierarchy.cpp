#include "engine/debug/SceneHierarchy.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <utility>

#include <glm/gtc/quaternion.hpp>
#include <imgui.h>
#include <imgui_stdlib.h>

#include "engine/component/ComponentManager2.h"
#include "engine/component/TransformComponent.h"
#include "engine/component/UIComponents.h"
#include "engine/core/EntityManager.h"
#include "engine/scene/SceneLoader.h"

namespace Debug {
CSceneHierarchy::CSceneHierarchy(Core::CEntityManager& entityManager,
                                 Component::CManager& componentManager,
                                 SEntitySelection& selection,
                                 Scene::CSceneLoader& sceneLoader)
    : mEntityManager(entityManager)
    , mComponentManager(componentManager)
    , mSelection(selection)
    , mSceneLoader(sceneLoader) {
    SetVisible(true);
}

void CSceneHierarchy::Render() {
    if (!ImGui::Begin("Scene Hierarchy", &mVisible)) {
        ImGui::End();
        return;
    }

    auto entities = mEntityManager.GetEntities();
    std::ranges::sort(entities, {}, [](const Core::SEntity& entity) {
        return entity.name.GetName();
    });

    if (ImGui::Button("+ Entity")) {
        CreateEntity(std::nullopt);
    }
    ImGui::SameLine();
    if (ImGui::Button("Save Scene")) {
        mSaveSucceeded = mSceneLoader.SaveCurrentScene();
        mSaveMessage = mSaveSucceeded ? "Scene saved" : "Failed to save scene";
    }
    if (!mSaveMessage.empty()) {
        ImGui::SameLine();
        ImGui::TextColored(mSaveSucceeded ? ImVec4(0.4f, 0.9f, 0.5f, 1.0f)
                                          : ImVec4(1.0f, 0.35f, 0.3f, 1.0f),
                           "%s", mSaveMessage.c_str());
    }
    ImGui::Button("Drop here to make a root", ImVec2(-1.0f, 24.0f));
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload =
                ImGui::AcceptDragDropPayload("ENTITY_NODE")) {
            if (payload->DataSize == sizeof(SEntityPayload)) {
                const auto* source =
                    static_cast<const SEntityPayload*>(payload->Data);
                const auto found = std::ranges::find_if(
                    entities, [source](const Core::SEntity& candidate) {
                        return candidate.id == source->id &&
                               candidate.generation == source->generation;
                    });
                if (found != entities.end()) {
                    ReparentEntity(*found, std::nullopt);
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    std::unordered_set<uint32_t> visited;
    auto& hierarchyPool = mComponentManager.GetPool<Component::SHierarchy>();

    for (const auto& entity : entities) {
        bool hasParent = false;
        if (hierarchyPool.Has(entity)) {
            const auto& hierarchy = hierarchyPool.Get(entity);
            const auto parent = mEntityManager.GetEntity(hierarchy.parent);
            hasParent = parent.has_value() && mEntityManager.IsAlive(*parent);
        }
        if (!hasParent) {
            RenderEntityNode(entity, entities, visited);
        }
    }

    for (const auto& entity : entities) {
        if (visited.contains(entity.id) || !hierarchyPool.Has(entity)) {
            continue;
        }

        std::vector<Core::SEntity> ancestry;
        std::unordered_map<uint32_t, std::size_t> ancestryIndices;
        auto current = std::optional<Core::SEntity>{entity};
        while (current && hierarchyPool.Has(*current)) {
            const auto [it, inserted] =
                ancestryIndices.emplace(current->id, ancestry.size());
            if (!inserted) {
                const auto cycleBegin = it->second;
                const auto cycleRoot = std::ranges::min_element(
                    ancestry.begin() + static_cast<std::ptrdiff_t>(cycleBegin),
                    ancestry.end(), {}, &Core::SEntity::id);
                if (cycleRoot != ancestry.end() && cycleRoot->id == entity.id) {
                    RenderEntityNode(entity, entities, visited);
                }
                break;
            }

            ancestry.push_back(*current);
            const auto& hierarchy = hierarchyPool.Get(*current);
            if (hierarchy.parent.GetName().empty()) {
                break;
            }
            current = mEntityManager.GetEntity(hierarchy.parent);
        }
    }

    if (ImGui::BeginPopupContextWindow("HierarchyContext",
                                       ImGuiPopupFlags_MouseButtonRight |
                                           ImGuiPopupFlags_NoOpenOverItems)) {
        if (ImGui::MenuItem("Add Entity")) {
            CreateEntity(std::nullopt);
        }
        ImGui::EndPopup();
    }
    RenderRenameModal();
    ImGui::End();
}

void CSceneHierarchy::RenderEntityNode(
    Core::SEntity entity, const std::vector<Core::SEntity>& entities,
    std::unordered_set<uint32_t>& visited) {
    if (!visited.insert(entity.id).second) {
        return;
    }

    auto& hierarchyPool = mComponentManager.GetPool<Component::SHierarchy>();
    std::vector<Core::SEntity> children;
    for (const auto& candidate : entities) {
        if (!hierarchyPool.Has(candidate)) {
            continue;
        }
        const auto& hierarchy = hierarchyPool.Get(candidate);
        if (hierarchy.parent == entity.name) {
            children.push_back(candidate);
        }
    }

    ImGui::PushID(static_cast<int>(entity.id));
    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (children.empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }
    if (mSelection.entity && mSelection.entity->id == entity.id &&
        mSelection.entity->generation == entity.generation) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    const std::string label = entity.name.GetName();
    const bool open = ImGui::TreeNodeEx(label.c_str(), flags);
    if (ImGui::IsItemClicked()) {
        mSelection.entity = entity;
    }
    if (ImGui::IsItemHovered() &&
        ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        mSelection.entity = entity;
        mRenameEntity = entity;
        mRenameBuffer = entity.name.GetName();
        mRenameError = false;
        mRenamePopupRequested = true;
        mRenameNeedsFocus = true;
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload =
                ImGui::AcceptDragDropPayload("ENTITY_NODE")) {
            if (payload->DataSize == sizeof(SEntityPayload)) {
                const auto* source =
                    static_cast<const SEntityPayload*>(payload->Data);
                const auto found = std::ranges::find_if(
                    entities, [source](const Core::SEntity& candidate) {
                        return candidate.id == source->id &&
                               candidate.generation == source->generation;
                    });
                if (found != entities.end()) {
                    ReparentEntity(*found, entity);
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::BeginDragDropSource()) {
        const SEntityPayload payload{entity.id, entity.generation};
        ImGui::SetDragDropPayload("ENTITY_NODE", &payload, sizeof(payload));
        ImGui::TextUnformatted(label.c_str());
        ImGui::EndDragDropSource();
    }

    const bool deleted = RenderEntityContextMenu(entity);

    if (open && !children.empty()) {
        if (!deleted) {
            for (const auto& child : children) {
                RenderEntityNode(child, entities, visited);
            }
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

bool CSceneHierarchy::RenderEntityContextMenu(Core::SEntity entity) {
    bool deleted = false;
    if (ImGui::BeginPopupContextItem("EntityContext")) {
        if (ImGui::MenuItem("Add Child Entity")) {
            CreateEntity(entity);
        }
        if (ImGui::MenuItem("Rename")) {
            mRenameEntity = entity;
            mRenameBuffer = entity.name.GetName();
            mRenameError = false;
            mRenamePopupRequested = true;
            mRenameNeedsFocus = true;
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Delete")) {
            DeleteEntity(entity);
            deleted = true;
        }
        ImGui::EndPopup();
    }
    return deleted;
}

void CSceneHierarchy::RenderRenameModal() {
    if (mRenamePopupRequested) {
        ImGui::OpenPopup("Rename Entity");
        mRenamePopupRequested = false;
    }
    if (!ImGui::BeginPopupModal("Rename Entity", nullptr,
                                ImGuiWindowFlags_AlwaysAutoResize)) {
        return;
    }

    ImGui::TextUnformatted("Entity name");
    ImGui::SetNextItemWidth(280.0f);
    if (mRenameNeedsFocus) {
        ImGui::SetKeyboardFocusHere();
        mRenameNeedsFocus = false;
    }
    const bool submitted =
        ImGui::InputText("##EntityName", &mRenameBuffer,
                         ImGuiInputTextFlags_EnterReturnsTrue |
                             ImGuiInputTextFlags_AutoSelectAll);
    if (mRenameError) {
        ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.3f, 1.0f),
                           "Name is empty or already in use.");
    }

    if (ImGui::Button("Rename") || submitted) {
        if (mRenameEntity && RenameEntity(*mRenameEntity, mRenameBuffer)) {
            mRenameEntity.reset();
            ImGui::CloseCurrentPopup();
        } else {
            mRenameError = true;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        mRenameEntity.reset();
        mRenameError = false;
        ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
}

Core::SEntity
CSceneHierarchy::CreateEntity(std::optional<Core::SEntity> parent) {
    std::string name = "Entity";
    for (uint32_t suffix = 1;
         mEntityManager.GetEntity(CStringId{name}).has_value(); ++suffix) {
        name = "Entity " + std::to_string(suffix + 1);
    }

    const Core::SEntity entity = mEntityManager.Create(CStringId{name});
    if (parent && mEntityManager.IsAlive(*parent)) {
        auto& hierarchyPool =
            mComponentManager.GetPool<Component::SHierarchy>();
        auto& hierarchy = hierarchyPool.Create(entity);
        hierarchy.parent = parent->name;

        if (hierarchyPool.Has(*parent)) {
            hierarchyPool.Update(*parent, [&](auto& parentHierarchy) {
                parentHierarchy.children.push_back(entity.name);
            });
        } else {
            auto& parentHierarchy = hierarchyPool.Create(*parent);
            parentHierarchy.children.push_back(entity.name);
        }
    }
    mSelection.entity = entity;
    return entity;
}

bool CSceneHierarchy::RenameEntity(Core::SEntity entity,
                                   const std::string& name) {
    if (!mEntityManager.Rename(entity, name)) {
        return false;
    }

    const CStringId oldName = mRenameEntity->name;
    const CStringId newName = entity.name;
    auto& hierarchyPool = mComponentManager.GetPool<Component::SHierarchy>();
    for (std::size_t index = 0; index < hierarchyPool.Size(); ++index) {
        const auto componentEntity = hierarchyPool.GetEntity(index);
        auto hierarchy = hierarchyPool.Get(componentEntity);
        bool changed = false;
        if (hierarchy.parent == oldName) {
            hierarchy.parent = newName;
            changed = true;
        }
        for (auto& child : hierarchy.children) {
            if (child == oldName) {
                child = newName;
                changed = true;
            }
        }
        if (changed) {
            hierarchyPool.Update(componentEntity,
                                 [&](auto& value) { value = hierarchy; });
        }
    }

    if (mSelection.entity && mSelection.entity->id == entity.id) {
        mSelection.entity = entity;
    }
    *mRenameEntity = entity;
    return true;
}

void CSceneHierarchy::DeleteEntity(Core::SEntity entity) {
    std::unordered_set<uint32_t> visited;
    DeleteEntityRecursive(entity, visited);
}

void CSceneHierarchy::DeleteEntityRecursive(
    Core::SEntity entity, std::unordered_set<uint32_t>& visited) {
    if (!mEntityManager.IsAlive(entity) || !visited.insert(entity.id).second) {
        return;
    }

    auto& hierarchyPool = mComponentManager.GetPool<Component::SHierarchy>();
    std::vector<Core::SEntity> children;
    for (const auto& candidate : mEntityManager.GetEntities()) {
        if (candidate.id != entity.id && hierarchyPool.Has(candidate) &&
            hierarchyPool.Get(candidate).parent == entity.name) {
            children.push_back(candidate);
        }
    }
    for (const auto& child : children) {
        DeleteEntityRecursive(child, visited);
    }

    for (std::size_t index = 0; index < hierarchyPool.Size(); ++index) {
        const auto componentEntity = hierarchyPool.GetEntity(index);
        if (componentEntity.id == entity.id) {
            continue;
        }
        auto hierarchy = hierarchyPool.Get(componentEntity);
        bool changed = false;
        if (hierarchy.parent == entity.name) {
            hierarchy.parent = CStringId{};
            changed = true;
        }
        const auto oldSize = hierarchy.children.size();
        std::erase(hierarchy.children, entity.name);
        changed |= hierarchy.children.size() != oldSize;
        if (changed) {
            hierarchyPool.Update(componentEntity,
                                 [&](auto& value) { value = hierarchy; });
        }
    }

    mEntityManager.Destroy(entity);
    if (mSelection.entity && mSelection.entity->id == entity.id) {
        mSelection.entity.reset();
    }
    if (mRenameEntity && mRenameEntity->id == entity.id) {
        mRenameEntity.reset();
        mRenamePopupRequested = false;
        mRenameNeedsFocus = false;
    }
}

bool CSceneHierarchy::ReparentEntity(Core::SEntity entity,
                                     std::optional<Core::SEntity> parent) {
    if (!mEntityManager.IsAlive(entity) ||
        (parent && !mEntityManager.IsAlive(*parent)) ||
        (parent && parent->id == entity.id)) {
        return false;
    }

    auto& hierarchyPool = mComponentManager.GetPool<Component::SHierarchy>();
    if (parent) {
        std::unordered_set<uint32_t> visited;
        auto current = parent;
        while (current && visited.insert(current->id).second) {
            if (current->id == entity.id) {
                return false;
            }
            if (!hierarchyPool.Has(*current)) {
                break;
            }
            const auto& currentHierarchy = hierarchyPool.Get(*current);
            if (currentHierarchy.parent.GetName().empty()) {
                break;
            }
            current = mEntityManager.GetEntity(currentHierarchy.parent);
        }
    }

    std::optional<Core::SEntity> oldParent;
    if (hierarchyPool.Has(entity)) {
        const auto& currentHierarchy = hierarchyPool.Get(entity);
        if (currentHierarchy.parent == (parent ? parent->name : CStringId{})) {
            return true;
        }
        if (!currentHierarchy.parent.GetName().empty()) {
            const auto found =
                mEntityManager.GetEntity(currentHierarchy.parent);
            if (found) {
                oldParent = *found;
            }
        }
    }

    if (oldParent && hierarchyPool.Has(*oldParent)) {
        hierarchyPool.Update(*oldParent, [&](auto& value) {
            std::erase(value.children, entity.name);
        });
    }

    if (hierarchyPool.Has(entity)) {
        hierarchyPool.Update(entity, [&](auto& value) {
            value.parent = parent ? parent->name : CStringId{};
        });
    } else {
        auto& hierarchy = hierarchyPool.Create(entity);
        hierarchy.parent = parent ? parent->name : CStringId{};
    }

    if (parent) {
        if (hierarchyPool.Has(*parent)) {
            hierarchyPool.Update(*parent, [&](auto& value) {
                if (std::ranges::find(value.children, entity.name) ==
                    value.children.end()) {
                    value.children.push_back(entity.name);
                }
            });
        } else {
            auto& hierarchy = hierarchyPool.Create(*parent);
            hierarchy.children.push_back(entity.name);
        }
    }
    return true;
}

CEntityInspector::CEntityInspector(Core::CEntityManager& entityManager,
                                   Component::CManager& componentManager,
                                   SEntitySelection& selection)
    : mEntityManager(entityManager)
    , mComponentManager(componentManager)
    , mSelection(selection) {
    SetVisible(true);
}

void CEntityInspector::Render() {
    if (!ImGui::Begin("Inspector", &mVisible)) {
        ImGui::End();
        return;
    }
    RenderSelectedEntity();
    ImGui::End();
}

void CEntityInspector::RenderSelectedEntity() {
    if (!mSelection.entity || !mEntityManager.IsAlive(*mSelection.entity)) {
        mSelection.entity.reset();
        ImGui::TextDisabled("Select an entity to inspect its components.");
        return;
    }

    const auto entity = *mSelection.entity;
    ImGui::Text("%s", entity.name.GetName().c_str());

    auto& localTransforms =
        mComponentManager.GetPool<Component::SLocalTransform>();
    if (localTransforms.Has(entity) &&
        ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        auto transform = localTransforms.Get(entity);
        bool changed =
            ImGui::DragFloat3("Position", &transform.position.x, 0.05f);
        glm::vec3 eulerDegrees =
            glm::degrees(glm::eulerAngles(transform.rotation));
        if (ImGui::DragFloat3("Rotation", &eulerDegrees.x, 0.5f)) {
            transform.rotation = glm::quat(glm::radians(eulerDegrees));
            changed = true;
        }
        changed |= ImGui::DragFloat3("Scale", &transform.scale.x, 0.05f);
        if (changed) {
            localTransforms.Update(entity,
                                   [&](auto& value) { value = transform; });
        }
    }

    auto& screenTransforms =
        mComponentManager.GetPool<Component::SScreenTransform>();
    if (screenTransforms.Has(entity)) {
        ImGui::SeparatorText("Screen Transform");
        auto value = screenTransforms.Get(entity);
        bool changed =
            ImGui::DragFloat2("Screen Position", &value.position.x, 0.5f);
        changed |= ImGui::DragFloat2("Screen Scale", &value.scale.x, 0.01f);
        float rotationDegrees = glm::degrees(value.rotation);
        if (ImGui::DragFloat("Screen Rotation", &rotationDegrees, 0.5f)) {
            value.rotation = glm::radians(rotationDegrees);
            changed = true;
        }
        changed |=
            ImGui::DragFloat2("Anchor", &value.anchor.x, 0.01f, 0.0f, 1.0f);
        if (changed) {
            screenTransforms.Update(entity,
                                    [&](auto& target) { target = value; });
        }
    }

    auto& sprites = mComponentManager.GetPool<Component::SSprite>();
    if (sprites.Has(entity)) {
        ImGui::SeparatorText("Sprite Renderer");
        auto value = sprites.Get(entity);
        bool changed = ImGui::InputText("Sprite", &value.spriteName);
        changed |= ImGui::DragFloat2("Sprite Size", &value.size.x, 1.0f);
        changed |= ImGui::ColorEdit4("Sprite Color", &value.color.x);
        changed |= ImGui::Checkbox("Maintain Aspect Ratio",
                                   &value.maintainAspectRatio);
        if (changed) {
            sprites.Update(entity, [&](auto& target) { target = value; });
        }
    }

    auto& texts = mComponentManager.GetPool<Component::SText>();
    if (texts.Has(entity)) {
        ImGui::SeparatorText("Text");
        const auto& current = texts.Get(entity);
        auto content = current.content;
        float fontSize = current.fontSize;
        auto color = current.color;
        bool animated = current.isAnimated;
        bool changed = ImGui::InputTextMultiline("Content", &content,
                                                 ImVec2(-1.0f, 70.0f));
        changed |= ImGui::DragFloat("Font Size", &fontSize, 0.5f, 1.0f, 256.0f);
        changed |= ImGui::ColorEdit4("Text Color", &color.x);
        changed |= ImGui::Checkbox("Animated", &animated);
        if (changed) {
            texts.Update(entity, [&](auto& value) {
                value.content = std::move(content);
                value.fontSize = fontSize;
                value.color = color;
                value.isAnimated = animated;
                value.dirty = true;
            });
        }
    }

    if (ImGui::Button("Add Screen Transform") &&
        !screenTransforms.Has(entity)) {
        screenTransforms.Create(entity);
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Sprite") && !sprites.Has(entity)) {
        sprites.Create(entity);
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Text") && !texts.Has(entity)) {
        auto& text = texts.Create(entity);
        text.fontId = 0;
        text.dirty = true;
    }
}

const char* CSceneHierarchy::GetName() const {
    return "Scene Hierarchy";
}

const char* CEntityInspector::GetName() const {
    return "Inspector";
}
} // namespace Debug
