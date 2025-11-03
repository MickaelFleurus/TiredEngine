#include "engine/scene/AbstractSceneHandler.h"
#include "engine/core/EngineLoop.h"

namespace Scene {

CAbstractSceneHandler::CAbstractSceneHandler(
    Core::CEngineLoop& engineLoop,
    Component::CComponentManager& componentManager,
    Core::CWindowData& windowData)
    : mEngineLoop(engineLoop)
    , mComponentManager(componentManager)
    , mWindowData(windowData) {
}

void CAbstractSceneHandler::SetCurrentScene(
    std::unique_ptr<Scene::CAbstractScene>&& scene) {
    mEngineLoop.SetCurrentScene(std::move(scene));
}

CAbstractScene* CAbstractSceneHandler::GetCurrentScene() const {
    return mEngineLoop.GetCurrentScene();
}

const std::vector<std::string>& CAbstractSceneHandler::GetSceneNames() const {
    return mSceneNames;
}

} // namespace Scene