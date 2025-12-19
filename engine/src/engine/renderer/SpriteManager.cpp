#include "engine/renderer/SpriteManager.h"

#include "engine/utils/SpriteSheetLoader.h"

namespace Renderer {
CSpriteManager::CSpriteManager(Utils::CFileHandler& fileHandler,
                               Renderer::CTextureManager& textureManager)
    : mFileHandler(fileHandler), mTextureManager(textureManager) {
}

CSpriteManager::~CSpriteManager() = default;

void CSpriteManager::LoadSpriteSheet(const std::string& spriteSheetName) {
    auto sprites =
        Utils::LoadSpriteSheet(mFileHandler, mTextureManager, spriteSheetName);
    mSprites.insert(sprites.begin(), sprites.end());
}
} // namespace Renderer
