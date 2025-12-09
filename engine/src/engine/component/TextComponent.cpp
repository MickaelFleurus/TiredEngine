#include "engine/component/TextComponent.h"

#include <glm/gtc/matrix_transform.hpp>

#include "engine/component/ComponentManager.h"
#include "engine/component/MovementComponent.h"
#include "engine/core/DataTypes.h"
#include "engine/core/GameObject.h"
#include "engine/font/Police.h"
#include "engine/renderer/TextRenderer.h"

namespace {
constexpr size_t kMaxCharacters = 64;
}

namespace Component {
CTextComponent::CTextComponent(Core::CGameObject& owner,
                               CComponentManager& componentManager)
    : IDisplayComponent(owner, componentManager,
                        Core::EDirtyType::TextInstanceProperties) {
}

CTextComponent::~CTextComponent() {
}

void CTextComponent::setText(const std::string& text) {
    mText = text;
    AddDirtyFlag(Core::EDirtyType::TextSizeChange);
}

void CTextComponent::setPolice(Font::CPolice* police) {
    mPolice = police;
    AddDirtyFlag(Core::EDirtyType::TextSizeChange);
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
    AddDirtyFlag(Core::EDirtyType::TextSizeChange);
}

const glm::vec4& CTextComponent::GetColor() const {
    return mColor;
}

void CTextComponent::SetColor(const glm::vec4& color) {
    mColor = color;
    AddDirtyFlag(Core::EDirtyType::TextInstanceProperties);
}

void CTextComponent::GenerateInstances() {
    mInstances.clear();

    if (!mPolice || mText.empty()) {
        return;
    }

    float cursorX = 0.0f;
    float cursorY = 0.0f;

    auto modelMatrix = mOwner.GetModelMatrix();

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

        float u = glyph.uv.x;
        float v = glyph.uv.y;
        float uw = glyph.uv.z;
        float vh = glyph.uv.w;
        glm::vec4 uvRect(u, v, uw, vh);

        Core::STextInstanceData instance;

        instance.textureId = mPolice->GetTextureIndex();
        instance.materialId = 0;
        instance.color = mColor;
        instance.modelMatrix =
            glm::translate(modelMatrix, glm::vec3(cursorX, cursorY, 0.0f));
        instance.uvRect = uvRect;
        mInstances.push_back(instance);

        cursorX += glyph.advance * mFontSize;
    }

    SetDirty(false);
}

glm::vec2 CTextComponent::GetSize() {
    if (!IsDirty()) {
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

const std::vector<Core::STextInstanceData>& CTextComponent::GetInstances() {
    if (IsDirty()) {
        GenerateInstances();
    }
    return mInstances;
}

} // namespace Component