#pragma once
#include <optional>
#include <string>
#include <variant>

#include "engine/component/IDisplayComponent.h"

namespace Component {
class CSpriteComponent : public IDisplayComponent {
public:
    explicit CSpriteComponent(Core::CGameObject& owner,
                              CComponentManager& componentManager);
    ~CSpriteComponent() override;
    // void AddSprite(SDK::SpriteType type);

    void Update(float deltaTime) override;
    glm::vec2 GetSize() override;

private:
    void removeSprite();
};
} // namespace Component