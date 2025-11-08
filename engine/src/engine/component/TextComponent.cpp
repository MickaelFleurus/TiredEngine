#include "engine/component/TextComponent.h"
#include "engine/component/ComponentManager.h"
#include "engine/component/MovementComponent.h"
#include "engine/core/GameObject.h"

#include "engine/renderer/TextRenderer.h"

#include "engine/font/Police.h"

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_log.h>

namespace {
constexpr size_t kMaxCharacters = 64;
}

namespace Component {
CTextComponent::CTextComponent(Core::CGameObject& owner,
                               CComponentManager& componentManager,
                               Renderer::CTextRenderer& textRenderer)
    : IComponent(owner, componentManager)
    , mTextRenderer(textRenderer)
    , mInstanceBuffer(mTextRenderer.CreateInstanceBuffer(kMaxCharacters)) {
}

CTextComponent::~CTextComponent() {
}

void CTextComponent::setText(const std::string& text) {
    mText = text;
    setDirty(true);
}

void CTextComponent::setPolice(Font::CPolice* police) {
    mPolice = police;
    setDirty(true);
}

Font::CPolice* CTextComponent::getPolice() const {
    return mPolice;
}

const std::string& CTextComponent::getText() const {
    return mText;
}

void CTextComponent::GenerateInstances() {
    mInstances.clear();

    if (!mPolice || mText.empty()) {
        return;
    }

    float cursorX = 0.0f;
    float cursorY = 0.0f;
    float fontSize = mPolice->GetSize();

    for (char c : mText) {
        const Font::GlyphInfo& glyph = mPolice->GetGlyphInfo(c);

        if (c == '\n') {
            cursorX = 0.0f;
            cursorY += fontSize;
            continue;
        }

        if (c == ' ') {
            cursorX += glyph.advance * fontSize;
            continue;
        }

        Renderer::SCharacterInstance instance;
        float glyphBottom = cursorY + glyph.offset.y * fontSize;
        float glyphCenterY = glyphBottom + (glyph.size.y * fontSize) * 0.5f;

        instance.position = glm::vec2(cursorX + glyph.offset.x * fontSize +
                                          (glyph.size.x * fontSize) * 0.5f,
                                      glyphCenterY);

        instance.size = glm::vec2(glyph.size.x, glyph.size.y) * fontSize;
        instance.uvRect = glyph.uv;
        instance.color = mPolice->GetColor();

        mInstances.push_back(instance);

        cursorX += glyph.advance * fontSize;
    }

    if (!mInstances.empty()) {
        mTextRenderer.UpdateInstanceBuffer(mInstanceBuffer, mInstances);
    }

    setDirty(false);
}

SDL_GPUBuffer* CTextComponent::GetInstanceBuffer() {
    if (isDirty()) {
        GenerateInstances();
    }
    return mInstanceBuffer;
}

uint32_t CTextComponent::GetInstanceCount() const {
    return static_cast<uint32_t>(mInstances.size());
}

} // namespace Component