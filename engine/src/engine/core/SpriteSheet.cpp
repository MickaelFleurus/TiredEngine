#include "engine/core/SpriteSheet.h"

#include <format>

#include "engine/renderer/TextureManager.h"
#include "engine/utils/FileHandler.h"
#include "engine/utils/Logger.h"

namespace {
constexpr const char* kSpriteFolder = "sprite_sheets";
}

namespace Core {
CSpriteSheetLoader::CSpriteSheetLoader(
    Utils::CFileHandler& fileHandler, Renderer::CTextureManager& textureManager,
    Renderer::CSpriteManager& spriteManager, const std::string& name)
    : mFileHandler(fileHandler)
    , mTextureManager(textureManager)
    , mSpriteManager(spriteManager)
    , mName(name) {
    const std::string spritePath = std::format(
        "{}/{}/{}", mFileHandler.GetTempFolder(), kSpriteFolder, name);

    if (!mFileHandler.DoesFileExist(spritePath, ".png") ||
        !mFileHandler.DoesFileExist(spritePath, ".json")) {
        LOG_ERROR("Sprite sheet files not found: {}", name);
        return;
    }
    int textureIndex = textureManager.LoadTexture(spritePath + ".png");
    auto spriteDesc = mFileHandler.LoadJson(spritePath + ".json");
    if (spriteDesc.is_null()) {
        LOG_ERROR("Failed to load sprite sheet description: {}", name);
        return;
    }
    if (!spriteDesc.contains("sprites")) {
        LOG_ERROR("Invalid sprite sheet description: {}", name);
        textureManager.UnloadTexture(textureIndex);
        textureIndex = -1;
        return;
    }
    for (auto& it : spriteDesc["sprites"].items()) {
        const auto& spriteInfo = it.value();
        if (!spriteInfo.contains("topLeft") || !spriteInfo.contains("size")) {
            LOG_WARNING("Invalid sprite info for '{}', skipping.", it.key());
            continue;
        }
        SSpriteInfo info;
        info.origin.x = spriteInfo["topLeft"]["x"];
        info.origin.y = spriteInfo["topLeft"]["y"];
        info.size.x = spriteInfo["size"]["x"];
        info.size.y = spriteInfo["size"]["y"];
        // mSprites[it.key()] = info;
    }
}

} // namespace Core
