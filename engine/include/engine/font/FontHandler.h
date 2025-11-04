#pragma once

#include "engine/font/Font.h"
#include "engine/font/Police.h"
#include <unordered_map>

struct TTF_Font;
struct SDL_GPUDevice;

namespace Utils {
class CFileHandler;
}

namespace Material {
class CMaterialFactory;
}

namespace Renderer {
class CTextureManager;
} // namespace Renderer

namespace Font {
class CFontHandler {
public:
    CFontHandler(Renderer::CTextureManager& textureManager,
                 Utils::CFileHandler& fileHandler,
                 Material::CMaterialFactory& materialFactory);
    ~CFontHandler();

    CPolice& GetPolice(const char* name, unsigned int size);

private:
    std::unordered_map<SFont, CPolice, SFontHash> mPolices;
    Renderer::CTextureManager& mTextureManager;
    Material::CMaterialFactory& mMaterialFactory;
    Utils::CFileHandler& mFileHandler;
};
} // namespace Font