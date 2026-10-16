#include "scene/GameSceneLoader.h"

#include <magic_enum/magic_enum.hpp>

#include "core/BrickType.h"


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
