#pragma once
#include "engine/component/IComponent.h"

#include <string>

namespace Component {
class CTextComponent : public IComponent {
public:
    explicit CTextComponent(Core::CGameObject& owner,
                            CComponentManager& componentManager);
    ~CTextComponent() override;
    // void addSprite(SDK::SpriteType type);
    void addText(const std::string& text, unsigned size);
    void updateText(const std::string& text);

private:
    std::string mText;
};
} // namespace Component