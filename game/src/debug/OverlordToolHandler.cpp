#include "debug/OverlordToolHandler.h"

#include <engine/core/GameObjectManager.h>
#include <engine/debug/Overlord.h>

namespace Debug {

COverlordToolHandler::COverlordToolHandler(
    Component::CComponentManager& componentManager,
    Utils::CFileHandler& fileHandler, Scene::ISceneHandler& sceneHandler,
    Font::CFontHandler& fontHandler, Vulkan::CBufferHandler& bufferHandler,
    Renderer::CTextureManager& textureManager,
    Scene::CSceneLoader& sceneLoaderOverlord,
    Core::CCameraManager& cameraManager,
    Core::CGameObjectManager& gameObjectManager)
    : mSettings(fileHandler, sceneHandler)
    , mOverlordSettings(mSettings, fileHandler, sceneHandler)
    , mSceneLoaderOverlord(sceneHandler)
    , mEntityWidget(componentManager, fileHandler, fontHandler,
                    gameObjectManager)
    , mSceneHierarchy(componentManager, mEntityWidget, sceneHandler,
                      gameObjectManager)
    , mBufferWidget(bufferHandler)
    , mTexturesWidget(textureManager)
    , mLevelSceneLoaderWidget(fileHandler, sceneLoaderOverlord)
    , mCamera3DWidget(cameraManager)
    , mCameraUIWidget(cameraManager) {
}

void COverlordToolHandler::Initialize() {
    mSettings.Initialize();
    RegisterTools();
    mSceneHierarchy.SetVisible(true);
}

void COverlordToolHandler::RegisterTools() {
    COverlord::AddMenu(mSceneLoaderOverlord, mToken);

    COverlord::AddWidget(mSceneHierarchy, mToken);
    COverlord::AddWidget(mBufferWidget, mToken);
    COverlord::AddWidget(mTexturesWidget, mToken);
    COverlord::AddWidget(mLevelSceneLoaderWidget, mToken);

    COverlord::AddMenu(mOverlordSettings, mToken);
    COverlord::AddWidget(mCamera3DWidget, mToken);
    COverlord::AddWidget(mCameraUIWidget, mToken);
}

} // namespace Debug
