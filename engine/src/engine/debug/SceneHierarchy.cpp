#include "engine/debug/SceneHierarchy.h"

#include <algorithm>
#include <string>

#include <glm/gtc/quaternion.hpp>
#include <imgui.h>
#include <imgui_stdlib.h>

#include "engine/component/ComponentManager2.h"
#include "engine/component/TransformComponent.h"
#include "engine/component/UIComponents.h"
#include "engine/core/EntityManager.h"

namespace Debug {
CSceneHierarchy::CSceneHierarchy(Core::CEntityManager& entityManager,
                                 Component::CManager& componentManager)
    : mEntityManager(entityManager), mComponentManager(componentManager) {
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

    ImGui::BeginChild("EntityTree",
                      ImVec2(0.0f, ImGui::GetContentRegionAvail().y * 0.48f),
                      true);
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
        if (!visited.contains(entity.id)) {
            RenderEntityNode(entity, entities, visited);
        }
    }
    ImGui::EndChild();

    ImGui::Separator();
    RenderSelectedEntity();
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
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                               ImGuiTreeNodeFlags_OpenOnDoubleClick |
                               ImGuiTreeNodeFlags_SpanAvailWidth;
    if (children.empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }
    if (mSelectedEntity && mSelectedEntity->id == entity.id &&
        mSelectedEntity->generation == entity.generation) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    const std::string label = entity.name.GetName();
    const bool open = ImGui::TreeNodeEx(label.c_str(), flags);
    if (ImGui::IsItemClicked()) {
        mSelectedEntity = entity;
    }

    if (open && !children.empty()) {
        for (const auto& child : children) {
            RenderEntityNode(child, entities, visited);
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

void CSceneHierarchy::RenderSelectedEntity() {
    if (!mSelectedEntity || !mEntityManager.IsAlive(*mSelectedEntity)) {
        mSelectedEntity.reset();
        ImGui::TextDisabled("Select an entity to inspect its components.");
        return;
    }

    const auto entity = *mSelectedEntity;
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
} // namespace Debug

