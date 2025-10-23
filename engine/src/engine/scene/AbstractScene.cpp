#include "engine/scene/AbstractScene.h"

namespace Scene {

CAbstractScene::CAbstractScene(Component::CComponentManager& componentManager,
                               const Core::CWindowData& windowData)
    : mComponentManager(componentManager)
    , mWindowData(windowData)
    , mGameObjectBuilder(componentManager) {
    mSceneRoot = std::unique_ptr<Core::CGameObject>(
        Core::CGameObjectBuilder::createRoot(componentManager));
}

Core::CGameObjectBuilder::CBuilder
CAbstractScene::CreateGameObjectBuilder(const std::string& name,
                                        Core::CGameObject* parent) {
    if (!parent) {
        parent = mSceneRoot.get();
    }
    return mGameObjectBuilder.CreateBuilder(name, *parent);
}

Core::CGameObject& CAbstractScene::GetRoot() {
    return *mSceneRoot;
}
} // namespace Scene