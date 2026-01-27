#include "engine/scene/SceneLoader.h"

#include "engine/core/EngineLoop.h"
#include "engine/renderer/SpriteManager.h"
#include "engine/renderer/TextureManager.h"
#include "engine/scene/AbstractScene.h"
#include "engine/utils/FileHandler.h"
#include "engine/utils/Logger.h"

namespace Scene {
CSceneLoader::CSceneLoader(Core::CEngineLoop& engineLoop,
                           Utils::CFileHandler& fileHandler,
                           Renderer::CTextureManager& textureManager,
                           Renderer::CSpriteManager& spriteManager)
    : mEngineLoop(engineLoop)
    , mFileHandler(fileHandler)
    , mTextureManager(textureManager)
    , mSpriteManager(spriteManager) {
}
CSceneLoader::~CSceneLoader() = default;

void CSceneLoader::LoadSceneFromFile(const std::filesystem::path& path) {
    auto yamlData = mFileHandler.LoadYAML(path);
    if (!yamlData.IsDefined()) {
        LOG_ERROR("Failed to load scene file: {}", path.string());
        return;
    }
    std::string sceneName = yamlData["name"].as<std::string>();
    // TODO
    // LOG_INFO("Loading scene: {}", sceneName);
    // if (yamlData["resources"].IsDefined()) {
    //     for (const auto& texture : yamlData["resources"]["textures"]) {
    //         std::string texturePath = mFileHandler.GetAssetsFolder() + "/" +
    //                                   texture.as<std::string>();

    //         mTextureManager.LoadTexture(texturePath);
    //         LOG_INFO("Loaded texture: {}", texturePath);
    //     }
    //     for (const auto& texture : yamlData["resources"]["spritesheets"]) {
    //         std::string texturePath = mFileHandler.GetAssetsFolder() + "/" +
    //                                   texture.as<std::string>();

    //         mSpriteManager.LoadSpriteSheet(texturePath);
    //         LOG_INFO("Loaded spritesheet: {}", texturePath);
    //     }
    // }
    // auto scene = LoadGameSpecificScenePart(yamlData["gameSpecific"]);
    // if (!scene) {
    //     LOG_FATAL("Failed to load game specific scene part");
    // }
    // mEngineLoop.SetPendingScene(std::move(scene));
}
} // namespace Scene
