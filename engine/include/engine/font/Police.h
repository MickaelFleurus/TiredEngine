#pragma once

#include "engine/font/FontTypes.h"

#include <glm/vec4.hpp>
#include <memory>
#include <string>
#include <unordered_map>

namespace Material {
class CAbstractMaterial;
}

namespace Font {
class CPolice {
public:
    struct SMetrics {
        double ascenderY;
        double descenderY;
        double lineHeight;
        double underlineY;
        double underlineThickness;
    };

    CPolice(const char* name, Material::CAbstractMaterial* material,
            int textureIndex,
            std::unordered_map<std::string, Font::GlyphInfo> glyphs,
            SMetrics fontMetrics);
    ~CPolice();

    const Font::GlyphInfo& GetGlyphInfo(char c) const;
    const SMetrics& GetFontMetrics() const;

    Material::CAbstractMaterial* GetMaterial();
    int GetTextureIndex() const;

    const std::string& GetName() const;

private:
    const std::string mName;
    Material::CAbstractMaterial* mMaterial;
    int mTextureIndex;

    std::unordered_map<std::string, Font::GlyphInfo> mGlyphs;
    SMetrics mFontMetrics;
};
} // namespace Font