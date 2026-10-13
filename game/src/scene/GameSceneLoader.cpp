#include "scene/GameSceneLoader.h"

#include <unordered_map>

#include <magic_enum/magic_enum.hpp>
#include <yaml-cpp/yaml.h>

#include "engine/utils/Logger.h"

#include "core/BrickType.h"
#include "scene/GameplayScene.h"

namespace {
struct SBrickDefinition {
    int hitPoints;
    Core::EBrickType type;
    Core::EBrickColor color;
};

} // namespace

namespace Scene {

CGameSceneLoader::CGameSceneLoader(Core::CEngineLoop& engineLoop,
                                   Utils::CFileHandler& fileHandler,
                                   Core::CEntityManager& entityManager,
                                   Component::CManager& componentManager,
                                   const System::CSystem& system)
    : CSceneLoader(engineLoop, fileHandler, entityManager, componentManager)

    , mSystem(system) {
}

} // namespace Scene
