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

void CTextComponent::updateText(const std::string& text,
                                Font::CPolice* police) {
    mText = text;
    mPolice = police;
}

void CTextComponent::setPolice(Font::CPolice* police) {
    mPolice = police;
}

Font::CPolice* CTextComponent::getPolice() const {
    return mPolice;
}

const std::string& CTextComponent::getText() const {
    return mText;
}

} // namespace Component