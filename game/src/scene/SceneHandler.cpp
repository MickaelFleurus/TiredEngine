#include "scene/SceneHandler.h"
#include "scene/SceneEnum.h"

#include "scene/DebugScene.h"
#include <magic_enum/magic_enum.hpp>

namespace Scene {
CSceneHandler::CSceneHandler(Core::CEngineLoop& engineLoop,
                             Component::CComponentManager& componentManager,
                             Core::CWindowData& windowData)
    : CAbstractSceneHandler(engineLoop, componentManager, windowData) {
}

void CSceneHandler::CreateAndSetScene(const std::string& sceneName) {
    auto sceneType = magic_enum::enum_cast<ESceneType>(sceneName);
    if (!sceneType.has_value()) {
        throw std::runtime_error("Scene name not recognized: " + sceneName);
    }

    switch (sceneType.value()) {
    case ESceneType::DEBUG:
        CreateAndSetSceneImpl<CDebugScene>();
        break;
    default:
        throw std::runtime_error("Scene type not implemented yet.");
    }
}
} // namespace Scene