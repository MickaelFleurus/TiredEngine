#include "engine/renderer/Police.h"

#include <SDL3_ttf/SDL_ttf.h>

namespace Renderer {
CPolice::CPolice(const char* fontPath, unsigned int fontSize)
    : mFontSize(fontSize), mFont(nullptr), mFontPath(fontPath) {
    // Load the font using SDL_gpu or any other font loading mechanism
    mFont = TTF_OpenFont(fontPath, fontSize);
}

CPolice::~CPolice() {
    UnloadFont();
}

void CPolice::SetFontSize(unsigned int fontSize) {
    mFontSize = fontSize;
}

unsigned int CPolice::GetFontSize() const {
    return mFontSize;
}

void CPolice::LoadFont() {
    UnloadFont();
    mFont = TTF_OpenFont(mFontPath.c_str(), mFontSize);
}

void CPolice::UnloadFont() {
    if (mFont) {
        TTF_CloseFont(mFont);
    }
}

} // namespace Renderer