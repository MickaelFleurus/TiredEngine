#pragma once
#include "engine/component/IDisplayComponent.h"

#include <optional>
#include <string>
#include <variant>

namespace Component {
class CSpriteComponent : public IDisplayComponent {
public:
    explicit CSpriteComponent(Core::CGameObject& owner,
                              CComponentManager& componentManager);
    ~CSpriteComponent() override;
    // void addSprite(SDK::SpriteType type);

    void update(float deltaTime) override;
    glm::vec2 getSize() override;

private:
    void removeSprite();
};
} // namespace Component