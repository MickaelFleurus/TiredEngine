#include "engine/font/Police.h"
#include "engine/renderer/TextureManager.h"

namespace Font {
CPolice::CPolice(Renderer::CTextureManager& textureManager,
                 const std::string name, unsigned int size,
                 std::span<Font::GlyphInfo> glyphs)
    : mTextureManager(textureManager)
    , mName(name)
    , mSize(size)
    , mGlyphs(glyphs.begin(), glyphs.end()) {
}

unsigned int CPolice::GetSize() const {
    return mSize;
}

SDL_GPUTexture* CPolice::GetAtlas() const {
    return mTextureManager.GetTexture(mName);
}

glm::vec4 CPolice::GetColor() const {
    return mColor;
}

void CPolice::SetColor(const glm::vec4& color) {
    mColor = color;
}

} // namespace Font
