#include "engine/scene/AbstractScene.h"
#include "engine/component/CameraComponent.h"
#include "engine/core/Camera.h"
#include "engine/core/WindowData.h"

namespace Scene {

CAbstractScene::CAbstractScene(Component::CComponentManager& componentManager,
                               Font::CFontHandler& fontHandler,
                               const Core::CWindowData& windowData)
    : mComponentManager(componentManager)
    , mWindowData(windowData)
    , mGameObjectBuilder(componentManager, fontHandler) {
    mSceneRoot = std::unique_ptr<Core::CGameObject>(
        Core::CGameObjectBuilder::createRoot(componentManager));
    mActiveCamera = std::make_unique<Core::CCamera>(
        *mSceneRoot, mGameObjectBuilder, mComponentManager);
    mActiveCamera->GetCameraComponent().SetViewportSize(
        glm::vec2(windowData.GetWidth(), windowData.GetHeight()));
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
} // namespace Scene