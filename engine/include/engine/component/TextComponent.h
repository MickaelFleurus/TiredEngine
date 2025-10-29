#pragma once
#include "engine/component/IComponent.h"

#include <string>

namespace Font {
class CPolice;
}

namespace Component {
class CTextComponent : public IComponent {
public:
    explicit CTextComponent(Core::CGameObject& owner,
                            CComponentManager& componentManager);
    ~CTextComponent() override;

    void updateText(const std::string& text, Font::CPolice* police = nullptr);
    void setPolice(Font::CPolice* police);

    Font::CPolice* getPolice() const;
    const std::string& getText() const;

private:
    std::string mText;
    Font::CPolice* mPolice;
};
} // namespace Component