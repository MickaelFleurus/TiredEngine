#include "engine/scene/SceneHandler.h"
#include "engine/core/EngineLoop.h"

namespace Scene {

CSceneHandler::CSceneHandler(Core::CEngineLoop& engineLoop,
                             Component::CComponentManager& componentManager,
                             Core::CWindowData& windowData)
    : mEngineLoop(engineLoop)
    , mComponentManager(componentManager)
    , mWindowData(windowData) {
}

void CSceneHandler::SetCurrentScene(
    std::unique_ptr<Scene::CAbstractScene>&& scene) {
    mEngineLoop.SetCurrentScene(std::move(scene));
}

CAbstractScene* CSceneHandler::GetCurrentScene() const {
    return mEngineLoop.GetCurrentScene();
}

} // namespace Scene