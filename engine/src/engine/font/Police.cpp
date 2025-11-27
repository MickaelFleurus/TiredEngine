#include "engine/font/Police.h"
#include "engine/material/AbstractMaterial.h"
#include "engine/renderer/DataTypes.h"

namespace Font {
CPolice::CPolice(const char* name,
                 std::unique_ptr<Material::AbstractMaterial> material,
                 std::unordered_map<std::string, Font::GlyphInfo> glyphs,
                 Utils::SBufferRange indexBufferRange,
                 Utils::SBufferRange vertexBufferRange,
                 CPolice::SMetrics fontMetrics)
    : mName(name)
    , mMaterial(std::move(material))
    , mGlyphs(std::move(glyphs))
    , mIndexBufferRange(indexBufferRange)
    , mVertexBufferRange(vertexBufferRange)
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

uint32_t CPolice::GetIndexOffset(char c) const {
    auto it = mGlyphs.find(std::string(1, c));
    if (it == mGlyphs.end()) {
        return 0; // FIXME this is bad but good enough for now
    }

    const int quadIndex = it->second.index;
    const int startIndex =
        mIndexBufferRange.offset / sizeof(uint32_t) + quadIndex * 6;

    return startIndex;
}

uint32_t CPolice::GetVertexOffset(char c) const {
    auto it = mGlyphs.find(std::string(1, c));
    if (it == mGlyphs.end()) {
        return 0;
    }

    const int quadIndex = it->second.index;
    const int startVertex =
        mVertexBufferRange.offset / sizeof(Renderer::SVertex) + quadIndex * 4;

    return startVertex;
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
