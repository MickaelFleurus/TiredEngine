#pragma once
#include "engine/component/IDisplayComponent.h"
#include "engine/renderer/DataTypes.h"
#include "engine/renderer/RendererUtils.h"

#include <string>
#include <vector>

namespace Font {
class CPolice;
}

namespace Renderer {
class CTextRenderer;
} // namespace Renderer

namespace Component {
class CTextComponent : public IDisplayComponent {
public:
    explicit CTextComponent(Core::CGameObject& owner,
                            CComponentManager& componentManager,
                            Renderer::CTextRenderer& textRenderer);
    ~CTextComponent() override;

    void setText(const std::string& text);
    void setPolice(Font::CPolice* police);

    Font::CPolice* getPolice() const;
    int GetFontSize() const;
    void SetFontSize(int size);
    const glm::vec4& GetColor() const;
    void SetColor(const glm::vec4& color);
    const std::string& getText() const;

    glm::vec2 getSize() override;

    const std::vector<Renderer::STextInstanceData>& GetInstances();

protected:
    virtual void setDirty(bool dirty) override;

private:
    void GenerateInstances();

    Renderer::CTextRenderer& mTextRenderer;
    std::vector<Renderer::STextInstanceData> mInstances;

    std::string mText;
    Font::CPolice* mPolice = nullptr;
    int mFontSize = 24;
    glm::vec2 mSize{0.0f, 0.0f};
    glm::vec4 mColor{1.0f, 1.0f, 1.0f, 1.0f};
};
} // namespace Component