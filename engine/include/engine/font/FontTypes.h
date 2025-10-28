#pragma once

#include <SDL3/SDL_rect.h>
#include <glm/vec2.hpp>

namespace Font {
struct GlyphInfo {
    SDL_Rect rect;
    int advance;      // How much to move cursor for next character
    glm::vec2 offset; // Glyph offset relative to baseline
};

} // namespace Font