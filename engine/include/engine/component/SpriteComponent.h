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
    explicit CSpriteComponent(Core::GameObjectId& owner,
                              CComponentManager& componentManager,
                              Renderer::CSpriteManager& spriteManager);
    CSpriteComponent(Core::GameObjectId& owner,
                     CComponentManager& componentManager,
                     const CSpriteComponent& other);

    CSpriteComponent& operator=(const CSpriteComponent& other);
    CSpriteComponent(const CSpriteComponent& other) = delete;
    CSpriteComponent(const CSpriteComponent&& other) = delete;
    CSpriteComponent& operator=(CSpriteComponent&& other) = delete;
    ~CSpriteComponent() override;
    void SetSprite(const std::string& spriteName);

    void Update(float deltaTime) override;
    glm::vec2 GetSize() const;

private:
    Renderer::CSpriteManager& mSpriteManager;
    std::optional<Core::SSpriteInfo> mCurrentSprite;
};
} // namespace Component
