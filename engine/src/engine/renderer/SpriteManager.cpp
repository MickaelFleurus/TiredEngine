#include "engine/renderer/SpriteManager.h"

#include <filesystem>

#include "engine/core/Sprite.h"
#include "engine/renderer/TextureManager.h"
#include "engine/utils/FileHandler.h"
#include "engine/utils/StringId.h"

#include "AtlasReader.h"

namespace {

Renderer::CSpriteManager::AtlasContainer
LoadSpritesheetsMetadata(std::filesystem::path path,
                         Renderer::CTextureManager& textureManager) {

    Renderer::CSpriteManager::AtlasContainer data;
    for (auto const& dir_entry : std::filesystem::directory_iterator{path}) {
        if (dir_entry.path().extension() != ".json") {
            continue;
        }

        std::unordered_map<CStringId, Core::SSpriteRendering, CStringIdHash>
            sprites;
        const auto metadata = AtlasReader::ParseMetadata(dir_entry.path());
        for (auto& atlas : metadata.atlases) {
            const auto loadedOpt =
                textureManager.LoadTexture(path / atlas.fileName);
            for (auto& sprite : atlas.entries) {
                Core::SSpriteRendering region{
                    .id = loadedOpt->second,
                    .uvMin =
                        glm::vec2(static_cast<float>(sprite.x) / atlas.width,
                                  static_cast<float>(sprite.y) / atlas.height),
                    .uvMax =
                        glm::vec2(static_cast<float>(sprite.x + sprite.width) /
                                      atlas.width,
                                  static_cast<float>(sprite.y + sprite.height) /
                                      atlas.height),
                    .pixelSize = {static_cast<float>(sprite.width),
                                  static_cast<float>(sprite.height)}};
                sprites.emplace(sprite.fileName, region);
            }
        }
        data.emplace(CStringId(metadata.name), sprites);
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

std::optional<Core::SSpriteRendering>
CSpriteManager::GetSpriteInfo(const std::string& spriteName) const {
    auto splitPos = spriteName.find("/");
    const CStringId atlasId = CStringId(spriteName.substr(0, splitPos));
    const CStringId spriteId = CStringId(spriteName.substr(splitPos + 1));
    if (mAtlases.contains(atlasId)) {
        const auto& atlas = mAtlases.at(atlasId);
        if (atlas.contains(spriteId)) {
            return atlas.at(spriteId);
        }
    }
    return std::nullopt;
}

} // namespace Renderer
