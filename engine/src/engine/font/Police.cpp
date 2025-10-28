#include "engine/font/Police.h"
#include <SDL3/SDL_gpu.h>

namespace Renderer {
CPolice::CPolice(SDL_GPUTexture* atlas, unsigned int size,
                 std::span<Font::GlyphInfo> glyphs)
    : mSize(size), mAtlas(atlas), mGlyphs(glyphs.begin(), glyphs.end()) {
}

unsigned int CPolice::GetSize() const {
    return mSize;
}

SDL_GPUTexture* CPolice::GetAtlas() const {
    return mAtlas;
}

glm::vec4 CPolice::GetColor() const {
    return mColor;
}

void CPolice::SetColor(const glm::vec4& color) {
    mColor = color;
}

} // namespace Renderer
