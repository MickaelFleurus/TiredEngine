#pragma once
#include <string>
#include <vector>

#include "engine/component/IComponent.h"
#include "engine/core/DataTypes.h"

namespace Font {
class CPolice;
}

namespace Component {
class CTextUIComponent : public IComponent {
public:
    explicit CTextUIComponent(Core::CGameObject& owner,
                              CComponentManager& componentManager);
    ~CTextUIComponent() override;

    void SetText(const std::string& text);
    void SetPolice(Font::CPolice* police);

    Font::CPolice* GetPolice() const;
    int GetFontSize() const;
    void SetFontSize(int size);
    const glm::vec4& GetColor() const;
    void SetColor(const glm::vec4& color);
    const std::string& GetText() const;

    glm::vec2 GetSize() const;

    const std::vector<Core::SUIInstanceData>& GetInstances();

private:
    void GenerateInstances();
    void ResolveSize();

    std::vector<Core::SUIInstanceData> mInstances;

    std::string mText;
    Font::CPolice* mPolice = nullptr;
    int mFontSize = 24;
    glm::vec2 mSize{0.0f, 0.0f};
    glm::vec4 mColor{1.0f, 1.0f, 1.0f, 1.0f};
};
} // namespace Component
