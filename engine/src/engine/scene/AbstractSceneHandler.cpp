#include "engine/scene/AbstractSceneHandler.h"
#include "engine/core/EngineLoop.h"

namespace Scene {

CAbstractSceneHandler::CAbstractSceneHandler(
    Core::CEngineLoop& engineLoop,
    Component::CComponentManager& componentManager,
    Font::CFontHandler& fontHandler, const System::CSystem& system)
    : mEngineLoop(engineLoop)
    , mComponentManager(componentManager)
    , mFontHandler(fontHandler)
    , mSystem(system) {
}

void CAbstractSceneHandler::SetCurrentScene(
    std::unique_ptr<Scene::CAbstractScene>&& scene) {
    mEngineLoop.SetCurrentScene(std::move(scene));
}

CAbstractScene* CAbstractSceneHandler::GetCurrentScene() const {
    return mEngineLoop.GetCurrentScene();
}

} // namespace Scene