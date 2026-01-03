#pragma once

#include <string>
#include <unordered_map>

#include "engine/font/Police.h"

namespace Utils {
class CFileHandler;
class CThreadPool;
} // namespace Utils

namespace Material {
class CMaterialManager;
}

namespace Renderer {
class CTextureManager;
} // namespace Renderer

namespace msdfgen {
class FreetypeHandle;
}

namespace Font {
class CFontHandler {
public:
    CFontHandler(Renderer::CTextureManager& textureManager,
                 Utils::CFileHandler& fileHandler,
                 Material::CMaterialManager& materialManager,
                 Utils::CThreadPool& threadPool);
    ~CFontHandler();

    CPolice& GetPolice(std::string name);
    void LoadAllThePolices();
    bool LoadFont(const std::string& name,
                  msdfgen::FreetypeHandle* freeType = nullptr);

private:
    std::unordered_map<std::string, CPolice> mPolices;
    Renderer::CTextureManager& mTextureManager;
    Utils::CFileHandler& mFileHandler;
    Material::CMaterialManager& mMaterialManager;
    Utils::CThreadPool& mThreadPool;
};
} // namespace Font
