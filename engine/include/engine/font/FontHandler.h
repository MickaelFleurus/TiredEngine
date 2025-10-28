#pragma once

#include "engine/font/Font.h"
#include "engine/font/Police.h"
#include <unordered_map>

struct TTF_Font;
struct SDL_GPUDevice;

namespace Utils {
class CFileHandler;
}

namespace Renderer {
class CFontHandler {
public:
    CFontHandler(SDL_GPUDevice* device, Utils::CFileHandler& fileHandler);
    ~CFontHandler();

    CPolice& GetPolice(const char* name, unsigned int size);

private:
    std::unordered_map<SFont, CPolice, SFontHash> mPolices;
    SDL_GPUDevice* mDevice{nullptr};
    Utils::CFileHandler& mFileHandler;
};
} // namespace Renderer