#pragma once

#include <string>
#include <vector>

#include "engine/core/Sprite.h"

namespace Utils {
class CFileHandler;
}

namespace Renderer {
class CTextureManager;
class CSpriteManager;
} // namespace Renderer

namespace Core {
class CSpriteSheetLoader {
public:
    CSpriteSheetLoader(Utils::CFileHandler& fileHandler,
                       Renderer::CTextureManager& textureManager,
                       Renderer::CSpriteManager& spriteManager,
                       const std::string& name);

private:
    Utils::CFileHandler& mFileHandler;
    Renderer::CTextureManager& mTextureManager;
    Renderer::CSpriteManager& mSpriteManager;
    const std::string mName;
};
} // namespace Core