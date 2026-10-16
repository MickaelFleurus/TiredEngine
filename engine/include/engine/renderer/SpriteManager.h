#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

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
    struct SLoadedSprite {
        std::string atlasName;
        std::string spriteName;
        Core::SSpriteRendering rendering;
    };

    using AtlasContainer = std::unordered_map<
        CStringId,
        std::unordered_map<CStringId, Core::SSpriteRendering, CStringIdHash>,
        CStringIdHash>;
    CSpriteManager(const CAssetParser& assetParser,
                   const Utils::CFileHandler& fileHandler,
                   Renderer::CTextureManager& textureManager);
    ~CSpriteManager();

    void Load();

    std::optional<Core::SSpriteRendering>
    GetSpriteInfo(const std::string& spriteName) const;
    std::vector<SLoadedSprite> GetLoadedSprites() const;

private:
    const CAssetParser& mAssetParser;
    const Utils::CFileHandler& mFileHandler;
    Renderer::CTextureManager& mTextureManager;

    AtlasContainer mAtlases;
};
} // namespace Renderer
