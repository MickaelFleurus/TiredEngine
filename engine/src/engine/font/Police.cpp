#include "engine/font/Police.h"
#include "engine/material/AbstractMaterial.h"

namespace Font {
CPolice::CPolice(const char* name,
                 std::unique_ptr<Material::AbstractMaterial> material,
                 unsigned int size, std::span<Font::GlyphInfo> glyphs)
    : mName(name)
    , mMaterial(std::move(material))
    , mSize(size)
    , mGlyphs(glyphs.begin(), glyphs.end()) {
}

CPolice::~CPolice() = default;

unsigned int CPolice::GetSize() const {
    return mSize;
}

glm::vec4 CPolice::GetColor() const {
    return mMaterial->GetColor();
}

void CPolice::SetColor(const glm::vec4& color) {
    mMaterial->SetColor(color);
}

const Font::GlyphInfo& CPolice::GetGlyphInfo(char c) const {

    size_t index = static_cast<size_t>(c) - 32; // TODO: Magic numbers, fix
    if (index < mGlyphs.size()) {
        return mGlyphs[index];
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
