#include "engine/debug/EntityWidget.h"

#include <imgui.h>

#include "engine/component/CameraComponent.h"
#include "engine/component/ComponentManager.h"
#include "engine/component/MeshComponent.h"
#include "engine/component/TextComponent.h"
#include "engine/component/TransformComponent.h"
#include "engine/debug/CameraWidget.h"
#include "engine/debug/MeshComponentWidget.h"
#include "engine/debug/TextComponentWidget.h"
#include "engine/debug/TransformComponentWidget.h"

namespace Debug {
CEntityWidget::CEntityWidget(Component::CComponentManager& componentManager,
                             Utils::CFileHandler& fileHandler,
                             Font::CFontHandler& fontHandler)
    : mComponentManager(componentManager)
    , mFileHandler(fileHandler)
    , mFontHandler(fontHandler) {
}

CEntityWidget::~CEntityWidget() = default;

void CEntityWidget::OnItemClicked(std::optional<int> entityId,
                                  std::string name) {
    mEntityId = entityId;
    mName = name;

    mTransformWidget.reset();
    mTextWidget.reset();
    mCameraWidget.reset();
    mMeshWidget.reset();

    if (!mEntityId) {
        SetVisible(false);
        return;
    }

    SetVisible(true);
    if (auto* transformComponent =
            mComponentManager.GetComponent<Component::CTransformComponent>(
                *mEntityId)) {
        mTransformWidget = std::make_unique<Debug::CTransformComponentWidget>(
            *transformComponent);
    }
    if (auto* textComponent =
            mComponentManager.GetComponent<Component::CTextComponent>(
                *mEntityId)) {
        mTextWidget = std::make_unique<Debug::CTextComponentWidget>(
            *textComponent, mFileHandler, mFontHandler);
    }
    if (auto* cameraComponent =
            mComponentManager.GetComponent<Component::CCameraComponent>(
                *mEntityId)) {
        mCameraWidget =
            std::make_unique<Debug::CCameraWidget>(*cameraComponent);
    }
    if (auto* meshComponent =
            mComponentManager.GetComponent<Component::CMeshComponent>(
                *mEntityId)) {
        mMeshWidget =
            std::make_unique<Debug::CMeshComponentWidget>(*meshComponent);
    }
}

void CEntityWidget::Render() {
    bool isVisible = IsVisible();

    // Position on right side of screen by default
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    if (!viewport) {
        return;
    }

    ImVec2 work_pos = viewport->WorkPos;
    ImVec2 work_size = viewport->WorkSize;

    ImVec2 window_pos;
    window_pos.x =
        work_pos.x + work_size.x - 420; // 20px padding from right edge
    window_pos.y = work_pos.y + 50;     // 50px from top

    ImGui::SetNextWindowPos(window_pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Entity", &isVisible)) {
        ImGui::Text("Name: %s", mName.c_str());
        if (mTransformWidget) {
            mTransformWidget->Render();
        }
        if (mTextWidget) {
            mTextWidget->Render();
        }
        if (mCameraWidget) {
            mCameraWidget->Render();
        }
        if (mMeshWidget) {
            mMeshWidget->Render();
        }
        SetVisible(isVisible);
    }
    ImGui::End();
}

const char* CEntityWidget::GetName() const {
    return "Entity Widget";
}

std::optional<int> CEntityWidget::GetEntityId() const {
    return mEntityId;
}
} // namespace Debug