#pragma once

#include <string>

struct TTF_Font;

namespace Renderer {
class CPolice {
public:
    CPolice(const char* fontPath, unsigned int fontSize);
    ~CPolice();

    void SetFontSize(unsigned int fontSize);
    unsigned int GetFontSize() const;

private:
    void LoadFont();
    void UnloadFont();

    unsigned int mFontSize;
    TTF_Font* mFont;
    const std::string mFontPath;
};
} // namespace Renderer