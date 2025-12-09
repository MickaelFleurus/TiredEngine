#include "engine/scene/AbstractScene.h"

#include "engine/component/CameraComponent.h"
#include "engine/core/UICamera.h"
#include "engine/system/System.h"

namespace Scene {

CAbstractScene::CAbstractScene(Component::CComponentManager& componentManager,
                               Font::CFontHandler& fontHandler,
                               Core::CMeshManager& meshManager,
                               const System::CSystem& system)
    : mComponentManager(componentManager)
    , mSystem(system)
    , mGameObjectBuilder(componentManager, fontHandler, meshManager) {
    const auto& windowData = mSystem.GetDisplayParameters();
    mSceneRoot = std::unique_ptr<Core::CGameObject>(
        Core::CGameObjectBuilder::CreateRoot(componentManager));

    mUICamera = std::make_unique<Core::CUICamera>(
        *mSceneRoot, mGameObjectBuilder, mComponentManager);
}

CAbstractScene::~CAbstractScene() = default;

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

Core::CCamera& CAbstractScene::GetActiveCamera() {
    return *mActiveCamera;
}

Core::CCamera& CAbstractScene::GetUICamera() {
    return *mUICamera;
}
} // namespace Scene