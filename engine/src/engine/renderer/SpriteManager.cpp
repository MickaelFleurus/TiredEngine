#include "engine/renderer/SpriteManager.h"

#include <filesystem>

#include "engine/core/Sprite.h"
#include "engine/renderer/TextureManager.h"
#include "engine/utils/FileHandler.h"
#include "engine/utils/StringId.h"

#include "AtlasReader.h"

namespace {

std::unordered_map<CStringId, Core::SSpriteAtlas, CStringIdHash>
LoadSpritesheetsMetadata(std::filesystem::path path,
                         Renderer::CTextureManager& textureManager) {

    std::unordered_map<CStringId, Core::SSpriteAtlas, CStringIdHash> data;
    for (auto const& dir_entry : std::filesystem::directory_iterator{path}) {
        if (dir_entry.path().extension() != ".json") {
            continue;
        }
        const auto metadata = AtlasReader::ParseMetadata(dir_entry.path());
        for (auto& atlas : metadata.atlases) {
            Core::SSpriteAtlas sprites;
            for (auto& sprite : atlas.entries) {
                Core::SSpriteRegion region{
                    .uvMin = glm::vec2(sprite.uvU, sprite.uvV),
                    .uvMax = glm::vec2(sprite.uvU + sprite.width,
                                       sprite.uvV + sprite.height)};
                sprites.sprites.emplace(sprite.fileName, region);
            }
            const auto loadedOpt =
                textureManager.LoadTexture(path / atlas.fileName);
            sprites.id = loadedOpt->second;
            data.emplace(CStringId(atlas.fileName), sprites);
        }
    }
    return data;
}

} // namespace

namespace Renderer {
CSpriteManager::CSpriteManager(const CAssetParser& assetParser,
                               const Utils::CFileHandler& fileHandler,
                               Renderer::CTextureManager& textureManager)
    : mAssetParser(assetParser)
    , mFileHandler(fileHandler)
    , mTextureManager(textureManager) {
}

CSpriteManager::~CSpriteManager() = default;

void CSpriteManager::Load() {

    mAtlases = LoadSpritesheetsMetadata(mFileHandler.GetAssetsFolder("sprites"),
                                        mTextureManager);
}

} // namespace Renderer
