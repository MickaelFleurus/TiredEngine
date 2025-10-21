#include "engine/scene/AbstractScene.h"

namespace Scene {

AbstractScene::AbstractScene(Component::CComponentManager& componentManager,
                             const Core::CWindowData& windowData)
    : mComponentManager(componentManager)
    , mWindowData(windowData)
    , mGameObjectBuilder(componentManager) {
    mSceneRoot = std::unique_ptr<Core::CGameObject>(
        Core::CGameObjectBuilder::createRoot(componentManager));
}

Core::CGameObjectBuilder::CBuilder
AbstractScene::CreateGameObjectBuilder(Core::CGameObject* parent) {
    if (!parent) {
        parent = mSceneRoot.get();
    }
    return mGameObjectBuilder.CreateBuilder(*parent);
}

Core::CGameObject& AbstractScene::GetRoot() {
    return *mSceneRoot;
}
} // namespace Scene