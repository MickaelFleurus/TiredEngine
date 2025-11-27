#include "engine/component/TextComponent.h"
#include "engine/component/ComponentManager.h"
#include "engine/component/MovementComponent.h"
#include "engine/core/GameObject.h"

#include "engine/font/Police.h"
#include "engine/renderer/DataTypes.h"
#include "engine/renderer/TextRenderer.h"

namespace {
constexpr size_t kMaxCharacters = 64;
}

namespace Component {
CTextComponent::CTextComponent(Core::CGameObject& owner,
                               CComponentManager& componentManager,
                               Renderer::CTextRenderer& textRenderer)
    : IDisplayComponent(owner, componentManager), mTextRenderer(textRenderer) {
    mTextRenderer.RegisterTextComponent(this);
}

CTextComponent::~CTextComponent() {
    mTextRenderer.UnregisterTextComponent(this);
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

int CTextComponent::GetFontSize() const {
    return mFontSize;
}

void CTextComponent::SetFontSize(int size) {
    mFontSize = size;
    setDirty(true);
}

const glm::vec4& CTextComponent::GetColor() const {
    return mColor;
}

void CTextComponent::SetColor(const glm::vec4& color) {
    mColor = color;
    setDirty(true);
}

void CTextComponent::GenerateInstances() {
    mInstances.clear();

    if (!mPolice || mText.empty()) {
        return;
    }

    float cursorX = 0.0f;
    float cursorY = 0.0f;

    auto modelMatrix = mOwner.getModelMatrix();

    for (char c : mText) {
        const Font::GlyphInfo& glyph = mPolice->GetGlyphInfo(c);

        if (c == '\n') {
            cursorX = 0.0f;
            cursorY += mFontSize * mPolice->GetFontMetrics().lineHeight;
            continue;
        }

        if (c == ' ') {
            cursorX += glyph.advance * mFontSize;
            continue;
        }

        Renderer::SInstanceData instance;

        instance.textureId = mPolice->GetTextureIndex();
        instance.materialId = 0;
        instance.color = mColor;
        instance.modelMatrix =
            glm::translate(modelMatrix, glm::vec3(cursorX, cursorY, 0.0f));
        mInstances.push_back(instance);

        cursorX += glyph.advance * mFontSize;
    }

    setDirty(false);
}

glm::vec2 CTextComponent::getSize() {
    if (!isDirty()) {
        return mSize;
    }

    float cursorX = 0.0f;
    float cursorY = 0.0f;
    float maxWidth = 0.0f;
    float totalHeight = mFontSize;

    for (char c : mText) {
        const Font::GlyphInfo& glyph = mPolice->GetGlyphInfo(c);

        if (c == '\n') {
            maxWidth = std::max(maxWidth, cursorX);
            cursorX = 0.0f;
            cursorY += mFontSize;
            totalHeight += mFontSize;
        } else {
            cursorX += glyph.advance * mFontSize;
        }
    }
    maxWidth = std::max(maxWidth, cursorX);

    mSize = glm::vec2(maxWidth, totalHeight);
    return mSize;
}

const std::vector<Renderer::SInstanceData>& CTextComponent::GetInstances() {
    if (isDirty()) {
        GenerateInstances();
    }
    return mInstances;
}

void CTextComponent::setDirty(bool dirty) {
    IComponent::setDirty(dirty);
    if (dirty) {
        mTextRenderer.SetNeedUpdate();
    }
}

} // namespace Component