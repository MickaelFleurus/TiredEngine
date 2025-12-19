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

std::optional<Core::SSpriteInfo>
CSpriteManager::GetSpriteInfo(const std::string& spriteName) const {
    auto it = mSprites.find(spriteName);
    if (it != mSprites.end()) {
        return it->second;
    }
    return std::nullopt;
}

} // namespace Renderer
