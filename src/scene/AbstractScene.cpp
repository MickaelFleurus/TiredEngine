#include "scene/AbstractScene.h"

namespace Scene {

AbstractScene::AbstractScene(Component::CComponentManager& componentManager,
                             const Core::SWindowData& windowData)
    : mComponentManager(componentManager)
    , mWindowData(windowData)
    , mGameObjectBuilder(componentManager) {
    mSceneRoot =
        std::unique_ptr<Core::CGameObject>(Core::CGameObjectBuilder::createRoot(componentManager));
}

Core::CGameObjectBuilder::CBuilder
AbstractScene::createGameObjectBuilder(Core::CGameObject* parent) {
    if (!parent) {
        parent = mSceneRoot.get();
    }
    return mGameObjectBuilder.createBuilder(*parent);
}

Core::CGameObject& AbstractScene::getRoot() {
    return *mSceneRoot;
}
} // namespace Scene