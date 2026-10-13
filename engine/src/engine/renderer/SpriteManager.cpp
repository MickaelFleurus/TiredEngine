#include "engine/renderer/SpriteManager.h"

#include "engine/utils/FileHandler.h"

namespace {

std::unordered_map<CStringId, Core::SSpriteAtlas, CStringIdHash>
LoadSpritesheetsMetadata(std::string path) {
}
} // namespace

namespace Renderer {
CSpriteManager::CSpriteManager(const CAssetParser& assetParser,
                               const Utils::CFileHandler& fileHandler,
                               Renderer::CTextureManager& textureManager)
    : mAssetParser(assetParser)
    , mFileHandler(fileHandler)
    , mTextureManager(textureManager)
    , mAtlases(
          LoadSpritesheetsMetadata(mFileHandler.GetAssetsFolder("sprites"))) {
}

CSpriteManager::~CSpriteManager() = default;

} // namespace Renderer
