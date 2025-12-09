#pragma once
#include <string>
#include <vector>

#include "engine/component/IDisplayComponent.h"
#include "engine/core/DataTypes.h"

namespace Font {
class CPolice;
}

namespace Component {
class CTextComponent : public IDisplayComponent {
public:
    explicit CTextComponent(Core::CGameObject& owner,
                            CComponentManager& componentManager);
    ~CTextComponent() override;

    void setText(const std::string& text);
    void setPolice(Font::CPolice* police);

    Font::CPolice* getPolice() const;
    int GetFontSize() const;
    void SetFontSize(int size);
    const glm::vec4& GetColor() const;
    void SetColor(const glm::vec4& color);
    const std::string& getText() const;

    glm::vec2 GetSize() override;

    const std::vector<Core::STextInstanceData>& GetInstances();

private:
    void GenerateInstances();

    std::vector<Core::STextInstanceData> mInstances;

    std::string mText;
    Font::CPolice* mPolice = nullptr;
    int mFontSize = 24;
    glm::vec2 mSize{0.0f, 0.0f};
    glm::vec4 mColor{1.0f, 1.0f, 1.0f, 1.0f};
};
} // namespace Component