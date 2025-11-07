#include "engine/font/Police.h"
#include "engine/material/AbstractMaterial.h"

namespace Font {
CPolice::CPolice(const char* name,
                 std::unique_ptr<Material::AbstractMaterial> material,
                 unsigned int size,
                 std::unordered_map<std::string, Font::GlyphInfo> glyphs,
                 CPolice::SMetrics fontMetrics)
    : mName(name)
    , mMaterial(std::move(material))
    , mSize(size)
    , mGlyphs(std::move(glyphs))
    , mFontMetrics(fontMetrics) {
}

CPolice::~CPolice() = default;

unsigned int CPolice::GetSize() const {
    return mSize;
}

void CPolice::SetSize(unsigned int size) {
    mSize = size;
}

glm::vec4 CPolice::GetColor() const {
    return mMaterial->GetColor();
}

void CPolice::SetColor(const glm::vec4& color) {
    mMaterial->SetColor(color);
}

const Font::GlyphInfo& CPolice::GetGlyphInfo(char c) const {
    if (mGlyphs.contains(std::string(1, c))) {
        return mGlyphs.at(std::string(1, c));
    } else {
        static Font::GlyphInfo emptyGlyph = {};
        return emptyGlyph;
    }
}

Material::AbstractMaterial& CPolice::GetMaterial() {
    return *mMaterial;
}

const std::string& CPolice::GetName() const {
    return mName;
}

} // namespace Font
