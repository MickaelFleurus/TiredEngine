#pragma once
#include "component/IComponent.h"

#include <optional>
#include <string>
#include <variant>

namespace Component {
class CSpriteComponent : public IComponent {
public:
    explicit CSpriteComponent(Core::CGameObject& owner,
                              CComponentManager& componentManager);
    ~CSpriteComponent() override;
    // void addSprite(SDK::SpriteType type);
    void addText(const std::string& text, unsigned size);
    void updateText(const std::string& text);
    void update(float deltaTime) override;

    void updatePosition();

private:
    void removeSprite();

    // struct SpriteIDWrapper {
    //     SDK::SpriteID id;
    // };
    // struct TextIDWrapper {
    //     SDK::TextID id;
    // };
    // std::optional<std::variant<SpriteIDWrapper, TextIDWrapper>> mSpriteId;

    // SDK::Window& mWindow;
};
} // namespace Component