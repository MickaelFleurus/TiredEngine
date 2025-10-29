#include "engine/debug/EntityWidget.h"

#include "engine/component/ComponentManager.h"
#include "engine/component/TextComponent.h"
#include "engine/component/TransformComponent.h"

#include "engine/debug/TextComponentWidget.h"
#include "engine/debug/TransformComponentWidget.h"

#include <imgui.h>

namespace Debug {
CEntityWidget::CEntityWidget(Component::CComponentManager& componentManager,
                             Utils::CFileHandler& fileHandler)
    : mComponentManager(componentManager), mFileHandler(fileHandler) {
}

CEntityWidget::~CEntityWidget() = default;

void CEntityWidget::OnItemClicked(std::optional<int> entityId) {
    mEntityId = entityId;

    mTransformWidget.reset();
    mTextWidget.reset();

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
            *textComponent, mFileHandler);
    }
}

void CEntityWidget::Render() {
    bool isVisible = IsVisible();
    if (ImGui::Begin("Entity Widget", &isVisible)) {
        if (mTransformWidget) {
            mTransformWidget->Render();
        }
        if (mTextWidget) {
            mTextWidget->Render();
        }
        SetVisible(isVisible);
    }
    ImGui::End();
}

const char* CEntityWidget::GetName() const {
    return "Entity Widget";
}
} // namespace Debug