#include "scene/SceneHandler.h"
#include "scene/SceneEnum.h"

#include "scene/DebugScene.h"
#include <engine/utils/Logger.h>
#include <magic_enum/magic_enum.hpp>

namespace Scene {
CSceneHandler::CSceneHandler(Core::CEngineLoop& engineLoop,
                             Component::CComponentManager& componentManager,
                             Font::CFontHandler& fontHandler,
                             Core::CMeshManager& meshManager,
                             const System::CSystem& system)
    : CAbstractSceneHandler(engineLoop, componentManager, fontHandler,
                            meshManager, system) {
}

void CSceneHandler::CreateAndSetScene(const std::string& sceneName) {
    auto sceneType = magic_enum::enum_cast<ESceneType>(sceneName);
    if (!sceneType.has_value()) {
        LOG_FATAL("Scene name not recognized: {}", sceneName);
    }

    switch (sceneType.value()) {
    case ESceneType::DEBUG:
        CreateAndSetSceneImpl<CDebugScene>();
        break;
    default:
        LOG_FATAL("Scene type not implemented yet: {}", sceneName);
    }
}

const std::vector<std::string>& CSceneHandler::GetSceneNames() const {
    static std::vector<std::string> sceneNames(
        magic_enum::enum_names<ESceneType>().begin(),
        magic_enum::enum_names<ESceneType>().end());
    return sceneNames;
}
} // namespace Scene