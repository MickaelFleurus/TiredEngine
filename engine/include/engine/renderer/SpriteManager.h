#pragma once
#include <optional>
#include <string>
#include <unordered_map>

#include "engine/core/Sprite.h"
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

    void LoadSpriteSheet(const std::string& spriteSheetName);

    std::optional<Core::SSpriteInfo>
    GetSpriteInfo(const std::string& spriteName) const;

private:
    const CAssetParser& mAssetParser;
    const Utils::CFileHandler& mFileHandler;
    Renderer::CTextureManager& mTextureManager;

    std::unordered_map<std::string, Core::SSpriteInfo> mSprites;
};
} // namespace Renderer
