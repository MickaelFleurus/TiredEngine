#include "scene/GameSceneLoader.h"

#include <unordered_map>

#include <magic_enum/magic_enum.hpp>
#include <yaml-cpp/yaml.h>

#include "engine/utils/Logger.h"

#include "core/BrickType.h"
#include "core/Grid.h"
#include "scene/GameplayScene.h"

namespace {
struct SBrickDefinition {
    int hitPoints;
    Core::EBrickType type;
    Core::EBrickColor color;
};

} // namespace

namespace Scene {

CGameSceneLoader::CGameSceneLoader(
    Core::CEngineLoop& engineLoop, Utils::CFileHandler& fileHandler,
    Renderer::CTextureManager& textureManager,
    Renderer::CSpriteManager& spriteManager,
    Component::CComponentManager& componentManager,
    Font::CFontHandler& fontHandler, Core::CMeshManager& meshManager,
    const System::CSystem& system)
    : CSceneLoader(engineLoop, fileHandler, textureManager, spriteManager)
    , mComponentManager(componentManager)
    , mFontHandler(fontHandler)
    , mMeshManager(meshManager)
    , mSystem(system) {
}

CGameSceneLoader::~CGameSceneLoader() = default;

std::unique_ptr<Scene::CAbstractScene>
CGameSceneLoader::LoadGameSpecificScenePart(const YAML::Node& sceneData) {
    std::unique_ptr<CGameplayScene> gameplayScene =
        std::make_unique<CGameplayScene>(mComponentManager, mFontHandler,
                                         mMeshManager, mSystem);
    if (!sceneData["grid"].IsDefined()) {
        LOG_FATAL("Gameplay scene must have a grid defined");
        return nullptr;
    }
    if (!sceneData["grid"]["brickDefinitions"].IsDefined()) {
        LOG_FATAL("Gameplay scene must have brick definitions defined");
        return nullptr;
    }

    auto& grid = gameplayScene->CreateGrid(
        sceneData["grid"]["size"]["width"].as<int>(),
        sceneData["grid"]["size"]["height"].as<int>());

    std::unordered_map<char, SBrickDefinition> brickTypeMap;

    for (const auto& brickDef : sceneData["grid"]["brickDefinitions"]) {
        Core::EBrickType type = magic_enum::enum_cast<Core::EBrickType>(
                                    brickDef["type"].as<std::string>())
                                    .value();
        Core::EBrickColor color = magic_enum::enum_cast<Core::EBrickColor>(
                                      brickDef["color"].as<std::string>())
                                      .value();
        int hitPoints = brickDef["hitPoints"].as<int>();
        char key = brickDef["key"].as<char>();
        brickTypeMap[key] = {hitPoints, type, color};
    }
    std::string layout = sceneData["grid"]["layout"].as<std::string>();
    for (const auto& c : layout) {
        if (c == '\n') {
            grid.NextRow();
        } else {
            auto it = brickTypeMap.find(c);
            if (it != brickTypeMap.end()) {
                const auto& def = it->second;
                grid.AddBrickToRow(def.hitPoints, def.type, def.color);
            }
        }
    }
    grid.GenerateGrid(Core::CGrid::EGridHorizontalAlignment::Center,
                      Core::CGrid::EGridVerticalAlignment::Center);

    return std::move(gameplayScene);
}

} // namespace Scene
