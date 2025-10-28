#pragma once

#include "engine/font/FontTypes.h"
#include <glm/vec4.hpp>
#include <span>
#include <string>

struct SDL_GPUTexture;
namespace Renderer {
class CTextureManager;
}

namespace Font {
class CPolice {
public:
    CPolice(Renderer::CTextureManager& textureManager, const std::string name,
            unsigned int size, std::span<Font::GlyphInfo> glyphs);

    unsigned int GetSize() const;
    SDL_GPUTexture* GetAtlas() const;
    glm::vec4 GetColor() const;

    void SetColor(const glm::vec4& color);

private:
    Renderer::CTextureManager& mTextureManager;
    const std::string mName;
    unsigned int mSize;
    std::vector<Font::GlyphInfo> mGlyphs;

    glm::vec4 mColor{1.0f, 1.0f, 1.0f, 1.0f};
};
} // namespace Font