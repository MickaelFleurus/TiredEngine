#include "engine/component/TextComponent.h"
#include "engine/component/ComponentManager.h"
#include "engine/component/MovementComponent.h"
#include "engine/core/GameObject.h"

#include <SDL3/SDL_gpu.h>

namespace Component {
CTextComponent::CTextComponent(Core::CGameObject& owner,
                               CComponentManager& componentManager)
    : IComponent(owner, componentManager) {
}

CTextComponent::~CTextComponent() {
}

void CTextComponent::addText(const std::string& text, unsigned size) {
    // mSpriteId = TextIDWrapper{mWindow.createText(text, size, 0.f, 0.f)};
}

void CTextComponent::updateText(const std::string& text) {
    // if (mSpriteId && std::holds_alternative<TextIDWrapper>(*mSpriteId)) {
    //     // mWindow.updateText(std::get<TextIDWrapper>(*mSpriteId).id, text);
    // }
}

} // namespace Component