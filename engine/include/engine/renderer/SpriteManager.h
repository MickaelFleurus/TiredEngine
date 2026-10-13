#pragma once
#include <optional>
#include <string>
#include <unordered_map>

#include "engine/core/Sprite.h"
#include "engine/utils/StringId.h"

namespace Utils {
class CFileHandler;
}

class CAssetParser;

namespace Renderer {
class CTextureManager;

class CSpriteManager {
public:
    CSpriteManager(const CAssetParser& assetParser,
                   const Utils::CFileHandler& fileHandler,
                   Renderer::CTextureManager& textureManager);
    ~CSpriteManager();

    std::optional<Core::SSpriteRegion>
    GetSpriteInfo(const std::string& spriteName) const;

private:
    const CAssetParser& mAssetParser;
    const Utils::CFileHandler& mFileHandler;
    Renderer::CTextureManager& mTextureManager;

    std::unordered_map<CStringId, Core::SSpriteAtlas, CStringIdHash> mAtlases;
};
} // namespace Renderer
