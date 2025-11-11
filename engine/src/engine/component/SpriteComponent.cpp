#include "engine/component/SpriteComponent.h"
#include "engine/component/ComponentManager.h"
#include "engine/component/MovementComponent.h"
#include "engine/core/GameObject.h"

namespace Component {
CSpriteComponent::CSpriteComponent(Core::CGameObject& owner,
                                   CComponentManager& componentManager)
    : IDisplayComponent(owner, componentManager) {
}

CSpriteComponent::~CSpriteComponent() {
    removeSprite();
}

void CSpriteComponent::removeSprite() {
}

void CSpriteComponent::update(float) {
}

glm::vec2 CSpriteComponent::getSize() {
    return glm::vec2(1.0f, 1.0f);
}

} // namespace Component