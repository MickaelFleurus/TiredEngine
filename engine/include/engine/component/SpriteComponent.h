#pragma once
#include <optional>
#include <string>
#include <variant>

#include "engine/component/IComponent.h"

namespace Component {
class CSpriteComponent : public IComponent {
public:
    explicit CSpriteComponent(Core::CGameObject& owner,
                              CComponentManager& componentManager);
    ~CSpriteComponent() override;
    // void AddSprite(SDK::SpriteType type);

    void Update(float deltaTime) override;
    glm::vec2 GetSize() const;

private:
    void removeSprite();
};
} // namespace Component
