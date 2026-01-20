#include "engine/debug/EntityWidget.h"

#include <imgui.h>
#include <magic_enum/magic_enum.hpp>

#include "engine/component/CameraComponent.h"
#include "engine/component/ComponentManager.h"
#include "engine/component/MeshComponent.h"
#include "engine/component/TextUIComponent.h"
#include "engine/core/GameObject.h"
#include "engine/debug/CameraComponentWidget.h"
#include "engine/debug/MeshComponentWidget.h"
#include "engine/debug/TextUIComponentWidget.h"
#include "engine/debug/TransformComponentWidget.h"
#include "engine/renderer/TransformManager.h"

namespace Debug {
CEntityWidget::CEntityWidget(Component::CComponentManager& componentManager,
                             Utils::CFileHandler& fileHandler,
                             Font::CFontHandler& fontHandler,
                             Renderer::CTransformManager& transformManager)
    : mComponentManager(componentManager)
    , mFileHandler(fileHandler)
    , mFontHandler(fontHandler)
    , mTransformManager(transformManager) {
}

CEntityWidget::~CEntityWidget() = default;

void CEntityWidget::OnItemClicked(std::optional<Core::GameObjectId> id) {
    mId = id;

    mTransformWidget.reset();
    mTextWidget.reset();
    mCameraWidget.reset();
    mMeshWidget.reset();

    // Reset expanded states
    mTransformExpanded = true;
    mTextUIExpanded = false;
    mCameraExpanded = false;
    mMeshExpanded = false;

    if (!mId) {
        SetVisible(false);
        return;
    }
    SetVisible(true);

    mTransformWidget = std::make_unique<Debug::CTransformWidget>(
        mTransformManager.CreateHandle(*mId));

    if (auto* textComponent =
            mComponentManager.GetComponent<Component::CTextUIComponent>(*mId)) {
        mTextWidget = std::make_unique<Debug::CTextUIComponentWidget>(
            *textComponent, mFileHandler, mFontHandler);
    }
    if (auto* cameraComponent =
            mComponentManager.GetComponent<Component::CCamera3DComponent>(
                *mId)) {
        mCameraWidget = std::make_unique<Debug::CCameraComponentWidget>();
    }
    if (auto* meshComponent =
            mComponentManager.GetComponent<Component::CMeshComponent>(*mId)) {
        mMeshWidget =
            std::make_unique<Debug::CMeshComponentWidget>(*meshComponent);
    }
}

void CEntityWidget::RenderEntityHeader() {
    // Entity name header with background
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));

    ImGui::BeginChild("EntityHeader", ImVec2(0, 50), true,
                      ImGuiWindowFlags_NoMove);
    ImGui::TextUnformatted("Entity Properties");
    ImGui::SameLine(ImGui::GetWindowWidth() - 120);
    ImGui::TextWrapped("Name: %s", "Unknown"); // TODO
    ImGui::EndChild();

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::Spacing();
}

void CEntityWidget::RenderComponentWithHeader(
    Component::EComponentType componentType, IOverlordItem* widget) {
    if (!widget) {
        return;
    }

    // Determine which state variable to use
    bool* expandedState = nullptr;
    if (componentType == Component::EComponentType::Transform) {
        expandedState = &mTransformExpanded;
    } else if (componentType == Component::EComponentType::TextUI) {
        expandedState = &mTextUIExpanded;
    } else if (componentType == Component::EComponentType::Camera) {
        expandedState = &mCameraExpanded;
    } else if (componentType == Component::EComponentType::Mesh) {
        expandedState = &mMeshExpanded;
    }

    if (!expandedState) {
        return;
    }

    // Component header with collapse/expand button
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered,
                          ImVec4(0.25f, 0.25f, 0.25f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.2f, 0.6f));

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
    if (*expandedState) {
        flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    }

    bool isOpen =
        ImGui::TreeNodeEx(magic_enum::enum_name(componentType).data(), flags);

    // Add remove button for non-Transform components
    if (componentType != Component::EComponentType::Transform) {
        ImGui::SameLine();
        ImGui::PushID(magic_enum::enum_name(componentType).data());
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4(0.9f, 0.2f, 0.2f, 1.0f));

        if (ImGui::SmallButton("X")) {
            ImGui::PopStyleColor(2);
            ImGui::PopID();
            ImGui::PopStyleColor(2);
            mComponentManager.RemoveComponent(*mId, componentType);
            OnItemClicked(mId);
            ImGui::Spacing();
            ImGui::TreePop();
            return;
        }

        ImGui::PopStyleColor(2);
        ImGui::PopID();
    }

    ImGui::PopStyleColor(2);

    if (isOpen) {
        ImGui::Indent();

        // Render component content
        widget->Render();

        ImGui::Unindent();
        *expandedState = true;
    } else {
        *expandedState = false;
    }
    ImGui::TreePop();

    ImGui::Spacing();
}

void CEntityWidget::RenderComponentsSection() {
    if (mTransformWidget) {
        RenderComponentWithHeader(Component::EComponentType::Transform,
                                  mTransformWidget.get());
    }
    if (mMeshWidget) {
        RenderComponentWithHeader(Component::EComponentType::Mesh,
                                  mMeshWidget.get());
    }
    if (mCameraWidget) {
        RenderComponentWithHeader(Component::EComponentType::Camera,
                                  mCameraWidget.get());
    }
    if (mTextWidget) {
        RenderComponentWithHeader(Component::EComponentType::TextUI,
                                  mTextWidget.get());
    }
}

void CEntityWidget::RenderAddComponentSection() {

    const float buttonWidth = (ImGui::GetContentRegionAvail().x - 8) * 0.5f;
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
    if (ImGui::Button("Add Component", ImVec2(buttonWidth, 0))) {
        ImGui::OpenPopup("AddComponentPopup");
    }
    ImGui::PopStyleColor(2);
}

void CEntityWidget::Render() {
    mVisible = mVisible && mId;

    // Position on right side of screen by default
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    if (!viewport) {
        return;
    }

    ImVec2 work_pos = viewport->WorkPos;
    ImVec2 work_size = viewport->WorkSize;

    ImVec2 window_pos;
    window_pos.x = work_pos.x + work_size.x - 450;
    window_pos.y = work_pos.y + 50;

    ImGui::SetNextWindowPos(window_pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(430, 700), ImGuiCond_FirstUseEver);

    // Set window style for inspector look
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.95f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.12f, 1.0f));

    if (ImGui::Begin("Inspector", &mVisible, ImGuiWindowFlags_NoMove)) {
        if (mId) {
            RenderEntityHeader();
            RenderComponentsSection();
            RenderAddComponentSection();
            RenderAddComponentPopup();
        }
        ImGui::End();
    }

    ImGui::PopStyleColor(2);
}

void CEntityWidget::RenderAddComponentPopup() {

    if (ImGui::BeginPopupContextWindow("AddComponentPopup")) {
        for (auto type : magic_enum::enum_values<Component::EComponentType>()) {
            bool hasComponent = mComponentManager.HasComponent(*mId, type);
            if (!hasComponent) {
                if (ImGui::MenuItem(magic_enum::enum_name(type).data())) {
                    mComponentManager.AddComponent(type, *mId);
                    OnItemClicked(mId);
                }
            }
        }

        ImGui::EndPopup();
    }
}

const char* CEntityWidget::GetName() const {
    return "Entity Widget";
}

bool CEntityWidget::IsSelected(Core::GameObjectId id) const {
    return mId == id;
}
} // namespace Debug
