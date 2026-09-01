// #include "engine/utils/SpriteSheetLoader.h"

// #include <format>

// #include "engine/core/Sprite.h"
// #include "engine/renderer/TextureManager.h"
// #include "engine/utils/AssetParser.h"
// #include "engine/utils/FileHandler.h"
// #include "engine/utils/Logger.h"

// namespace {
// constexpr const char* kSpriteFolder = "sprite_sheets";
// }

// namespace Utils {

// std::unordered_map<std::string, Core::SSpriteInfo> LoadSpriteSheet(
//     const CAssetParser& assetParser, const CFileHandler& fileHandler,
//     Renderer::CTextureManager& textureManager, const std::string& name) {
//     std::unordered_map<std::string, Core::SSpriteInfo> sprites;
//     auto asset = assetParser.Get(EAssetType::Texture, name);
//     if (!asset || !asset->get().mMetadata.has_value()) {
//         LOG_ERROR("Could not locate file named {}", name);
//         return {};
//     }

//     int textureIndex = textureManager.LoadTexture(asset->get());
//     auto spriteDesc = fileHandler.LoadYAML(asset->get().mMetadata->string());
//     if (!spriteDesc.IsDefined()) {
//         LOG_ERROR("Failed to load sprite sheet description: {}", name);
//         return {};
//     }

//     std::string textureName = spriteDesc["name"].as<std::string>();
//     std::string texturePath = spriteDesc["path"].as<std::string>();

//     for (const auto sprite : spriteDesc["sprites"]) {
//         glm::ivec2 origin{
//             sprite["x"].as<int>(),
//             sprite["y"].as<int>(),
//         };
//         glm::ivec2 size{
//             sprite["width"].as<int>(),
//             sprite["height"].as<int>(),
//         };
//         Core::SSpriteInfo info{
//             .origin = origin,
//             .size = size,
//         };

//         std::string spriteName = sprite["name"].as<std::string>();
//         sprites[spriteName] = info;
//     }
//     return sprites;
// }

// } // namespace Utils
