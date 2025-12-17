#include "engine/font/Police.h"

#include "engine/material/AbstractMaterial.h"

namespace Font {
CPolice::CPolice(const char* name, int textureIndex,
                 std::unordered_map<std::string, Font::GlyphInfo> glyphs,
                 CPolice::SMetrics fontMetrics)
    : mName(name)
    , mTextureIndex(textureIndex)
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

int CPolice::GetTextureIndex() const {
    return mTextureIndex;
}

const std::string& CPolice::GetName() const {
    return mName;
}

} // namespace Font
