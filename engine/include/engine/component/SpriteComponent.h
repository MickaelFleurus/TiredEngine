#pragma once
#include <optional>
#include <string>

#include "engine/component/IComponent.h"
#include "engine/core/Sprite.h"

namespace Renderer {
class CSpriteManager;
}

namespace Component {
class CSpriteComponent : public IComponent {
public:
    explicit CSpriteComponent(Core::CGameObject& owner,
                              CComponentManager& componentManager,
                              Renderer::CSpriteManager& spriteManager);
    ~CSpriteComponent() override;
    void SetSprite(const std::string& spriteName);

    void Update(float deltaTime) override;
    glm::vec2 GetSize() const;

private:
    Renderer::CSpriteManager& mSpriteManager;
    std::optional<Core::SSpriteInfo> mCurrentSprite;
};
} // namespace Component
