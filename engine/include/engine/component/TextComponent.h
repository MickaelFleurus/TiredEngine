#pragma once
#include "engine/component/IComponent.h"

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
class CTextComponent : public IComponent {
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

private:
    void GenerateInstances();

    Renderer::CTextRenderer& mTextRenderer;
    SDL_GPUBuffer* mInstanceBuffer = nullptr;
    std::vector<Renderer::SCharacterInstance> mInstances;

    std::string mText;
    Font::CPolice* mPolice = nullptr;
};
} // namespace Component