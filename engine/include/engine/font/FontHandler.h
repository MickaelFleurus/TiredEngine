#pragma once

#include "engine/font/Font.h"
#include "engine/font/Police.h"
#include "engine/renderer/RendererUtils.h"
#include <unordered_map>

namespace Utils {
class CFileHandler;
}

namespace Material {
class CMaterialFactory;
}

namespace Renderer {
class CTextureManager;
class CBufferHandler;
class CBufferHandle;
} // namespace Renderer

namespace Font {
class CFontHandler {
public:
    CFontHandler(Renderer::CTextureManager& textureManager,
                 Utils::CFileHandler& fileHandler,
                 Material::CMaterialFactory& materialFactory,
                 Renderer::CBufferHandler& bufferHandler);
    ~CFontHandler();

    CPolice& GetPolice(const char* name, unsigned int size);

private:
    std::unordered_map<SFont, CPolice, SFontHash> mPolices;
    Renderer::CTextureManager& mTextureManager;
    Utils::CFileHandler& mFileHandler;
    Material::CMaterialFactory& mMaterialFactory;
    Renderer::CBufferHandler& mBufferHandler;

    Renderer::CBufferHandle* mBufferHandle = nullptr;
};
} // namespace Font