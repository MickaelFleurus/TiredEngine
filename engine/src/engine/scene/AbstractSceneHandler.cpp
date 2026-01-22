#include "engine/scene/AbstractSceneHandler.h"

#include "engine/core/EngineLoop.h"

namespace Scene {

CAbstractSceneHandler::CAbstractSceneHandler(
    Core::CEngineLoop& engineLoop,
    Component::CComponentManager& componentManager,
    Font::CFontHandler& fontHandler, Core::CMeshManager& meshManager,
    const System::CSystem& system, Core::CGameObjectManager& gameObjectManager)
    : mEngineLoop(engineLoop)
    , mComponentManager(componentManager)
    , mFontHandler(fontHandler)
    , mMeshManager(meshManager)
    , mSystem(system)
    , mGameObjectManager(gameObjectManager) {
}

void CAbstractSceneHandler::SetCurrentScene(
    std::unique_ptr<Scene::CAbstractScene>&& scene) {
    mEngineLoop.SetPendingScene(std::move(scene));
}

CAbstractScene* CAbstractSceneHandler::GetCurrentScene() const {
    return mEngineLoop.GetCurrentScene();
}

} // namespace Scene
