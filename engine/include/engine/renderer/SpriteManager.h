#pragma once
#include <string>
#include <unordered_map>

#include "engine/core/Sprite.h"

namespace Utils {
class CFileHandler;
} // namespace Utils

namespace Renderer {
class CTextureManager;

class CSpriteManager {
public:
    CSpriteManager(Utils::CFileHandler& fileHandler,
                   Renderer::CTextureManager& textureManager);
    ~CSpriteManager();

    void LoadSpriteSheet(const std::string& spriteSheetName);

private:
    Utils::CFileHandler& mFileHandler;
    Renderer::CTextureManager& mTextureManager;

    std::unordered_map<std::string, Core::SSpriteInfo> mSprites;
};
} // namespace Renderer
