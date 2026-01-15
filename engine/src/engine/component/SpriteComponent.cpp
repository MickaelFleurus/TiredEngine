#include "engine/component/SpriteComponent.h"

#include "engine/core/GameObject.h"
#include "engine/renderer/SpriteManager.h"

namespace Component {
CSpriteComponent::CSpriteComponent(Core::CGameObject& owner,
                                   CComponentManager& componentManager,
                                   Renderer::CSpriteManager& spriteManager)
    : IComponent(owner, componentManager, Core::EDirtyType::None)
    , mSpriteManager(spriteManager) {
}

CSpriteComponent::CSpriteComponent(Core::CGameObject& owner,
                                   CComponentManager& componentManager,
                                   const CSpriteComponent& other)
    : CSpriteComponent(owner, componentManager, other.mSpriteManager) {
    *this = other;
}

CSpriteComponent& CSpriteComponent::operator=(const CSpriteComponent& other) {
    if (other.mCurrentSprite.has_value()) {
        mCurrentSprite = other.mCurrentSprite;
        AddDirtyFlag(Core::EDirtyType::InstanceProperties);
    }
    return *this;
}

CSpriteComponent::~CSpriteComponent() = default;

void CSpriteComponent::SetSprite(const std::string& spriteName) {
    mCurrentSprite = mSpriteManager.GetSpriteInfo(spriteName);
    AddDirtyFlag(Core::EDirtyType::InstanceProperties);
}

void CSpriteComponent::Update(float) {
}

glm::vec2 CSpriteComponent::GetSize() const {
    return mCurrentSprite.has_value()
               ? glm::vec2{static_cast<float>(mCurrentSprite->size.x),
                           static_cast<float>(mCurrentSprite->size.y)}
               : glm::vec2{0.0f, 0.0f};
}

} // namespace Component
