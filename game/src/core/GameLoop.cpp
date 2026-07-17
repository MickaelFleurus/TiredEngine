#include "core/GameLoop.h"

#include "engine/system/System.h"
#include "engine/utils/Asserts.h"

namespace Core {
CGameLoop::CGameLoop(System::CSystem& system, Vulkan::SContext& context,
                     Vulkan::CSwapchain& swapchain)
    : CEngineLoop(system, context, swapchain)
    , mSceneHandler(*this, mComponentManager, mFontHandler, mMeshManager,
                    system, mGameObjectManager)
    , mSceneLoader(*this, system.GetFileHandler(), mTextureManager,
                   mSpriteManager, mComponentManager, mFontHandler,
                   mMeshManager, system, mGameObjectManager)
    , mToolHandler(mComponentManager, system.GetFileHandler(), mSceneHandler,
                   mFontHandler, mBufferHandler, mTextureManager, mSceneLoader,
                   mCameraManager, mGameObjectManager, mMeshManager,
                   mAssetParser) {

    mOverlordManager.CreateOverlord(context.window.get());
    mToolHandler.Initialize();
    mFontHandler.LoadAllThePolices();
    mSpriteManager.LoadSpriteSheet("bricks");
}

void CGameLoop::GameLoop(float deltaTime) {
    // Game-specific update logic goes here
}
} // namespace Core
