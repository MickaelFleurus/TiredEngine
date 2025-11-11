#pragma once
#include "engine/component/IDisplayComponent.h"

#include <string>
#include <vector>

struct SDL_GPUBuffer;

namespace Font {
class CPolice;
}

namespace Renderer {
class CTextRenderer;
struct SCharacterInstance;
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
    const std::string& getText() const;

    SDL_GPUBuffer* GetInstanceBuffer();
    uint32_t GetInstanceCount() const;

    glm::vec2 getSize() override;

private:
    void GenerateInstances();

    Renderer::CTextRenderer& mTextRenderer;
    SDL_GPUBuffer* mInstanceBuffer = nullptr;
    std::vector<Renderer::SCharacterInstance> mInstances;

    std::string mText;
    Font::CPolice* mPolice = nullptr;
    glm::vec2 mSize{0.0f, 0.0f};
};
} // namespace Component