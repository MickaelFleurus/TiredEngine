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

    for (char c : mText) {
        const Font::GlyphInfo& glyph = mPolice->GetGlyphInfo(c);
        if (c == '\n') {
            cursorX = 0.0f;
            cursorY += mPolice->GetSize();
            continue;
        }

        if (c == ' ') {
            cursorX += glyph.advance;
            continue;
        }

        Renderer::SCharacterInstance instance;

        // Add world position to character position
        instance.position =
            glm::vec2(cursorX + glyph.offset.x, cursorY + glyph.offset.y);

        instance.size = glm::vec2(static_cast<float>(glyph.size.x),
                                  static_cast<float>(glyph.size.y));

        instance.uvRect = glyph.uv;

        instance.color = mPolice->GetColor();

        mInstances.push_back(instance);

        cursorX += glyph.advance;
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