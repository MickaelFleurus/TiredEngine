#pragma once

#include <string>
#include <unordered_map>

#include <glm/vec4.hpp>

#include "engine/font/FontTypes.h"

namespace Font {
class CPolice {
public:
    struct SMetrics {
        double emSize;
        double ascenderY;
        double descenderY;
        double lineHeight;
        double underlineY;
        double underlineThickness;
    };

    CPolice(const char* name, uint64_t textureIndex,
            std::unordered_map<std::string, Font::GlyphInfo> glyphs,
            SMetrics fontMetrics);
    ~CPolice();

    const Font::GlyphInfo& GetGlyphInfo(char c) const;
    const SMetrics& GetFontMetrics() const;

    uint64_t GetTextureIndex() const;

    const std::string& GetName() const;

private:
    const std::string mName;
    uint64_t mTextureIndex;

    std::unordered_map<std::string, Font::GlyphInfo> mGlyphs;
    SMetrics mFontMetrics;
};
} // namespace Font
