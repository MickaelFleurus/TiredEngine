#pragma once

#include <expected>
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
    CFontHandler(Utils::CFileHandler& fileHandler, Thread::CPool& threadPool,
                 const CAssetParser& assetParser,
                 Renderer::CTextureManager& textureManager);
    ~CFontHandler();

    CPolice& GetPolice(const std::string& name);
    std::optional<std::reference_wrapper<CPolice>> GetPolice(std::size_t id);
    void LoadAllThePolices();
    bool LoadFont(const SAsset& fontAsset,
                  msdfgen::FreetypeHandle* freeType = nullptr);

private:
    std::vector<CPolice> mPolices;
    std::unordered_map<std::string, std::size_t> mPoliceNameToIndex;

    Utils::CFileHandler& mFileHandler;
    Thread::CPool& mThreadPool;
    const CAssetParser& mAssetParser;
    Renderer::CTextureManager& mTextureManager;
};
} // namespace Font
