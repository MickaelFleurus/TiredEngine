#pragma once

#include "engine/font/FontTypes.h"
#include "engine/utils/BufferMemoryBlocks.h"
#include <glm/vec4.hpp>
#include <memory>
#include <string>
#include <unordered_map>

namespace Material {
class AbstractMaterial;
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

    CPolice(const char* name,
            std::unique_ptr<Material::AbstractMaterial> material,
            std::unordered_map<std::string, Font::GlyphInfo> glyphs,
            Utils::SBufferRange vertexBufferRange,
            Utils::SBufferRange indexBufferRange, SMetrics fontMetrics);
    ~CPolice();

    const Font::GlyphInfo& GetGlyphInfo(char c) const;
    const SMetrics& GetFontMetrics() const {
        return mFontMetrics;
    }

    uint32_t GetIndexOffset(char c) const;
    uint32_t GetVertexOffset(char c) const;

    Material::AbstractMaterial& GetMaterial();
    int GetTextureIndex() const;

    const std::string& GetName() const;

private:
    const std::string mName;
    std::unique_ptr<Material::AbstractMaterial> mMaterial;

    std::unordered_map<std::string, Font::GlyphInfo> mGlyphs;
    Utils::SBufferRange mVertexBufferRange;
    Utils::SBufferRange mIndexBufferRange;
    SMetrics mFontMetrics;
};
} // namespace Font