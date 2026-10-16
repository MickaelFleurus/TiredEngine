#include "engine/font/Police.h"

#include "engine/material/AbstractMaterial.h"

namespace Font {
CPolice::CPolice(const char* name, uint64_t textureIndex,
                 std::unordered_map<std::string, Font::GlyphInfo> glyphs,
                 CPolice::SMetrics fontMetrics)
    : mName(name)
    , mTextureIndex(textureIndex)
    , mGlyphs(std::move(glyphs))
    , mFontMetrics(fontMetrics) {
}

CPolice::~CPolice() = default;

std::optional<Font::GlyphInfo> CPolice::GetGlyphInfo(char c) const {
    if (mGlyphs.contains(std::string(1, c))) {
        return mGlyphs.at(std::string(1, c));
    }
    return std::nullopt;
}

const CPolice::SMetrics& CPolice::GetFontMetrics() const {
    return mFontMetrics;
}

uint64_t CPolice::GetTextureIndex() const {
    return mTextureIndex;
}

const std::string& CPolice::GetName() const {
    return mName;
}

} // namespace Font
