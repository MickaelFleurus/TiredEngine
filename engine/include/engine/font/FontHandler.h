#pragma once

#include <string>
#include <unordered_map>

#include "engine/font/Police.h"

namespace Utils {
class CFileHandler;
} // namespace Utils

namespace Thread {
class CPool;
} // namespace Thread

namespace Material {
class CMaterialManager;
}

namespace Renderer {
class CTextureManager;
} // namespace Renderer

namespace msdfgen {
class FreetypeHandle;
}
struct SAsset;
class CAssetParser;

namespace Font {
class CFontHandler {
public:
    CFontHandler(Renderer::CTextureManager& textureManager,
                 Utils::CFileHandler& fileHandler,
                 Material::CMaterialManager& materialManager,
                 Thread::CPool& threadPool, const CAssetParser& assetParser);
    ~CFontHandler();

    CPolice& GetPolice(std::string name);
    void LoadAllThePolices();
    bool LoadFont(const SAsset& fontAsset,
                  msdfgen::FreetypeHandle* freeType = nullptr);

private:
    std::unordered_map<std::string, CPolice> mPolices;
    Renderer::CTextureManager& mTextureManager;
    Utils::CFileHandler& mFileHandler;
    Material::CMaterialManager& mMaterialManager;
    Thread::CPool& mThreadPool;
    const CAssetParser& mAssetParser;
};
} // namespace Font
