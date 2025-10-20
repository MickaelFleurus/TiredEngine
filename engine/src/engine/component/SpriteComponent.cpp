#include "engine/component/SpriteComponent.h"
#include "engine/component/ComponentManager.h"
#include "engine/component/MovementComponent.h"
#include "engine/core/GameObject.h"

namespace Component {
CSpriteComponent::CSpriteComponent(Core::CGameObject& owner,
                                   CComponentManager& componentManager)
    : IComponent(owner, componentManager) {
}

CSpriteComponent::~CSpriteComponent() {
    removeSprite();
}

void CSpriteComponent::addText(const std::string& text, unsigned size) {
    removeSprite();
    // mSpriteId = TextIDWrapper{mWindow.createText(text, size, 0.f, 0.f)};
    updatePosition();
}

void CSpriteComponent::updateText(const std::string& text) {
    // if (mSpriteId && std::holds_alternative<TextIDWrapper>(*mSpriteId)) {
    //     // mWindow.updateText(std::get<TextIDWrapper>(*mSpriteId).id, text);
    // }
}

void CSpriteComponent::removeSprite() {
    // if (mSpriteId) {
    //     if (std::holds_alternative<SpriteIDWrapper>(*mSpriteId)) {
    //         mWindow.removeSprite(std::get<SpriteIDWrapper>(*mSpriteId).id);
    //     } else if (std::holds_alternative<TextIDWrapper>(*mSpriteId)) {
    //         mWindow.removeText(std::get<TextIDWrapper>(*mSpriteId).id);
    //     }
    //     mSpriteId.reset();
    // }
}

void CSpriteComponent::update(float) {
}

void CSpriteComponent::updatePosition() {
    // if (mSpriteId) {
    //     const auto position = mOwner.getWorldPosition();
    //     if (std::holds_alternative<SpriteIDWrapper>(*mSpriteId)) {
    //         mWindow.moveSprite(std::get<SpriteIDWrapper>(*mSpriteId).id,
    //         position.x, position.y);
    //     } else if (std::holds_alternative<TextIDWrapper>(*mSpriteId)) {
    //         mWindow.moveText(std::get<TextIDWrapper>(*mSpriteId).id,
    //         position.x, position.y);
    //     }
    // }
}

} // namespace Component