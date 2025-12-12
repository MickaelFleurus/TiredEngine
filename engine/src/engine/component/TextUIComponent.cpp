#include "engine/component/TextUIComponent.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "engine/component/ComponentManager.h"
#include "engine/component/MovementComponent.h"
#include "engine/core/DataTypes.h"
#include "engine/core/GameObject.h"
#include "engine/font/Police.h"
#include "engine/renderer/TextRenderer.h"
#include "engine/utils/Logger.h"

namespace {
constexpr size_t kMaxCharacters = 64;
}

namespace Component {
CTextUIComponent::CTextUIComponent(Core::CGameObject& owner,
                                   CComponentManager& componentManager)
    : IDisplayComponent(owner, componentManager,
                        Core::EDirtyType::TextInstanceProperties) {
}

CTextUIComponent::~CTextUIComponent() {
}

void CTextUIComponent::SetText(const std::string& text) {
    mText = text;
    AddDirtyFlag(Core::EDirtyType::TextSizeChange);
}

void CTextUIComponent::SetPolice(Font::CPolice* police) {
    mPolice = police;
    AddDirtyFlag(Core::EDirtyType::TextSizeChange);
}

Font::CPolice* CTextUIComponent::GetPolice() const {
    return mPolice;
}

const std::string& CTextUIComponent::GetText() const {
    return mText;
}

int CTextUIComponent::GetFontSize() const {
    return mFontSize;
}

void CTextUIComponent::SetFontSize(int size) {
    mFontSize = size;
    AddDirtyFlag(Core::EDirtyType::TextSizeChange);
}

const glm::vec4& CTextUIComponent::GetColor() const {
    return mColor;
}

void CTextUIComponent::SetColor(const glm::vec4& color) {
    mColor = color;
    AddDirtyFlag(Core::EDirtyType::TextInstanceProperties);
}

void CTextUIComponent::GenerateInstances() {
    mInstances.clear();

    if (!mPolice || mText.empty()) {
        return;
    }

    const auto& metrics = mPolice->GetFontMetrics();
    float scale = mFontSize / metrics.emSize;

    float baselineY = metrics.ascenderY * scale;
    float cursorX = 0.0f;
    float cursorY = 0.0f;
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    for (char c : mText) {
        const Font::GlyphInfo& glyph = mPolice->GetGlyphInfo(c);

        if (c == '\n') {
            cursorX = 0.0f;
            cursorY += scale * metrics.lineHeight;
            continue;
        }

        if (c == ' ') {
            cursorX += glyph.advance * scale;
            continue;
        }

        glm::vec4 uvRect(glyph.uv.x, glyph.uv.y, glyph.uv.z, glyph.uv.w);

        Core::SUIInstanceData instance;
        instance.textureId = mPolice->GetTextureIndex();
        instance.materialId = 0;
        instance.color = mColor;

        glm::vec3 glyphPos(cursorX + glyph.offset.x * scale,
                           cursorY + baselineY - glyph.offset.y * scale -
                               glyph.size.y * scale,
                           0.0f);
        instance.modelMatrix = glm::translate(modelMatrix, glyphPos);
        instance.modelMatrix = glm::scale(
            instance.modelMatrix,
            glm::vec3(glyph.size.x * scale, glyph.size.y * scale, 1.0f));
        instance.uvRect = uvRect;
        mInstances.push_back(instance);

        cursorX += glyph.advance * scale;
    }

    SetDirty(false);
}

glm::vec2 CTextUIComponent::GetSize() {
    if (!IsDirty() && mSize != glm::vec2(0.0f)) {
        return mSize;
    }

    float scale = mFontSize / mPolice->GetFontMetrics().emSize;
    float cursorX = 0.0f;
    float cursorY = 0.0f;
    float maxWidth = 0.0f;
    float totalHeight = scale;

    for (char c : mText) {
        const Font::GlyphInfo& glyph = mPolice->GetGlyphInfo(c);

        if (c == '\n') {
            maxWidth = std::max(maxWidth, cursorX);
            cursorX = 0.0f;
            cursorY += scale * mPolice->GetFontMetrics().lineHeight;
            totalHeight += scale * mPolice->GetFontMetrics().lineHeight;
        } else {
            cursorX += glyph.advance * scale;
        }
    }
    maxWidth = std::max(maxWidth, cursorX);

    mSize = glm::vec2(maxWidth, totalHeight);
    return mSize;
}

const std::vector<Core::SUIInstanceData>& CTextUIComponent::GetInstances() {
    if (IsDirty()) {
        GenerateInstances();
    }
    return mInstances;
}

} // namespace Component