#include "core/GameLoop.h"

#include "engine/debug/Overlord.h"
#include "engine/system/System.h"

namespace Core {
CGameLoop::CGameLoop(System::CSystem& system, Vulkan::SContext& context,
                     Vulkan::CSwapchain& swapchain)
    : CEngineLoop(system, context, swapchain)
    , mSceneHandler(*this, system)
    , mSceneLoader(*this, system.GetFileHandler(), mEntityManager,
                   mComponentManager, system)
    , mSceneMenu(mSceneHandler)
    , mCamera3DWidget(mCameraManager)
    , mCameraUIWidget(mCameraManager)
    , mLevelSceneLoaderWidget(mAssetParser, mSceneLoader)
    , mBufferWidget(mBufferHandler)
    , mSceneHierarchy(mEntityManager, mComponentManager)
    , mTexturesWidget(mTextureManager)
    , mSpritesWidget(mSpriteManager, mTextureManager) {

    mSceneMenu.SetVisible(true);
    Debug::COverlord::AddMenu(mSceneMenu, mOverlordToken);
    Debug::COverlord::AddWidget(mCamera3DWidget, mOverlordToken);
    Debug::COverlord::AddWidget(mCameraUIWidget, mOverlordToken);
    Debug::COverlord::AddWidget(mLevelSceneLoaderWidget, mOverlordToken);
    Debug::COverlord::AddWidget(mBufferWidget, mOverlordToken);
    Debug::COverlord::AddWidget(mSceneHierarchy, mOverlordToken);
    Debug::COverlord::AddWidget(mTexturesWidget, mOverlordToken);
    Debug::COverlord::AddWidget(mSpritesWidget, mOverlordToken);

    mOverlordManager.CreateOverlord(context.window.get());
    mSceneLoader.LoadSceneFromFile(std::format(
        "{}/debug.json", system.GetFileHandler().GetAssetsFolder("scene")));

    mFontHandler.LoadAllThePolices();
}

void CGameLoop::GameLoop(float deltaTime) {
    // Game-specific update logic goes here
}
} // namespace Core
