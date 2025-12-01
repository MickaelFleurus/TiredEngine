#include "engine/font/Police.h"
#include "engine/material/AbstractMaterial.h"
#include "engine/renderer/DataTypes.h"

namespace Font {
CPolice::CPolice(const char* name,
                 std::unique_ptr<Material::AbstractMaterial> material,
                 std::unordered_map<std::string, Font::GlyphInfo> glyphs,
                 CPolice::SMetrics fontMetrics)
    : mName(name)
    , mMaterial(std::move(material))
    , mGlyphs(std::move(glyphs))
    , mFontMetrics(fontMetrics) {
}

CPolice::~CPolice() = default;

const Font::GlyphInfo& CPolice::GetGlyphInfo(char c) const {
    if (mGlyphs.contains(std::string(1, c))) {
        return mGlyphs.at(std::string(1, c));
    } else {
        static Font::GlyphInfo emptyGlyph = {};
        return emptyGlyph;
    }
}

const CPolice::SMetrics& CPolice::GetFontMetrics() const {
    return mFontMetrics;
}

Material::AbstractMaterial& CPolice::GetMaterial() {
    return *mMaterial;
}

int CPolice::GetTextureIndex() const {
    return mMaterial->GetTextureIndex();
}

const std::string& CPolice::GetName() const {
    return mName;
}

} // namespace Font
