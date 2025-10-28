#pragma once

#include "engine/font/FontTypes.h"
#include <glm/vec4.hpp>
#include <span>
#include <string>

struct SDL_GPUTexture;

namespace Renderer {
class CPolice {
public:
    CPolice(SDL_GPUTexture* atlas, unsigned int size,
            std::span<Font::GlyphInfo> glyphs);

    unsigned int GetSize() const;
    SDL_GPUTexture* GetAtlas() const;
    glm::vec4 GetColor() const;

    void SetColor(const glm::vec4& color);

private:
    unsigned int mSize;

    SDL_GPUTexture* mAtlas;
    std::vector<Font::GlyphInfo> mGlyphs;

    glm::vec4 mColor{1.0f, 1.0f, 1.0f, 1.0f};
};
} // namespace Renderer