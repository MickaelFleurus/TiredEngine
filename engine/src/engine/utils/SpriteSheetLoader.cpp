#include "engine/utils/SpriteSheetLoader.h"

#include <format>

#include "engine/core/Sprite.h"
#include "engine/renderer/TextureManager.h"
#include "engine/utils/FileHandler.h"
#include "engine/utils/Logger.h"

namespace {
constexpr const char* kSpriteFolder = "sprite_sheets";
}

namespace Utils {

std::unordered_map<std::string, Core::SSpriteInfo>
LoadSpriteSheet(Utils::CFileHandler& fileHandler,
                Renderer::CTextureManager& textureManager,
                const std::string& name) {
    std::unordered_map<std::string, Core::SSpriteInfo> sprites;
    const std::string spritePath = std::format(
        "{}/{}/{}", fileHandler.GetTempFolder(), kSpriteFolder, name);

    if (!fileHandler.DoesFileExist(spritePath, ".png") ||
        !fileHandler.DoesFileExist(spritePath, ".yaml")) {
        LOG_ERROR("Sprite sheet files not found: {}", name);
        return {};
    }
    int textureIndex = textureManager.LoadTexture(spritePath + ".png");
    auto spriteDesc = fileHandler.LoadYAML(spritePath + ".yaml");
    if (spriteDesc.IsDefined()) {
        LOG_ERROR("Failed to load sprite sheet description: {}", name);
        return {};
    }

    std::string textureName = spriteDesc["name"].as<std::string>();
    std::string texturePath = spriteDesc["path"].as<std::string>();

    for (const auto sprite : spriteDesc["sprites"]) {
        Core::SSpriteInfo info{
            .origin.x = sprite["x"].as<int>(),
            .origin.y = sprite["y"].as<int>(),
            .size.x = sprite["width"].as<int>(),
            .size.y = sprite["height"].as<int>(),
        };

        std::string spriteName = sprite["name"].as<std::string>();
        sprites[spriteName] = info;
    }
    return sprites;
}

} // namespace Utils
