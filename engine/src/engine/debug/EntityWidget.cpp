#include "engine/debug/EntityWidget.h"

#include "engine/component/ComponentManager.h"
#include "engine/component/TextComponent.h"
#include "engine/component/TransformComponent.h"

#include "engine/debug/CameraWidget.h"
#include "engine/debug/TextComponentWidget.h"
#include "engine/debug/TransformComponentWidget.h"

#include <imgui.h>

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

    if (!mEntityId) {
        SetVisible(false);
        return;
    }

    SetVisible(true);
    if (auto* transformComponent =
            mComponentManager.getComponent<Component::CTransformComponent>(
                *mEntityId)) {
        mTransformWidget = std::make_unique<Debug::CTransformComponentWidget>(
            *transformComponent);
    }
    if (auto* textComponent =
            mComponentManager.getComponent<Component::CTextComponent>(
                *mEntityId)) {
        mTextWidget = std::make_unique<Debug::CTextComponentWidget>(
            *textComponent, mFileHandler, mFontHandler);
    }
    if (auto* cameraComponent =
            mComponentManager.getComponent<Component::CCameraComponent>(
                *mEntityId)) {
        mCameraWidget =
            std::make_unique<Debug::CCameraWidget>(*cameraComponent);
    }
}

void CEntityWidget::Render() {
    bool isVisible = IsVisible();

    // Position on right side of screen by default
    ImGuiViewport* viewport = ImGui::GetMainViewport();
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