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
    CFontHandler(Utils::CFileHandler& fileHandler, Thread::CPool& threadPool,
                 const CAssetParser& assetParser);
    ~CFontHandler();

    CPolice& GetPolice(std::string name);
    void LoadAllThePolices();
    bool LoadFont(const SAsset& fontAsset,
                  msdfgen::FreetypeHandle* freeType = nullptr);

private:
    std::unordered_map<std::string, CPolice> mPolices;

    Utils::CFileHandler& mFileHandler;
    Thread::CPool& mThreadPool;
    const CAssetParser& mAssetParser;
};
} // namespace Font
