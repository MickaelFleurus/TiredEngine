#include "core/GameLoop.h"

#include "engine/system/System.h"
#include "engine/utils/Asserts.h"

namespace Core {
CGameLoop::CGameLoop(System::CSystem& system, SDL_Window* window,
                     Vulkan::CVulkanContext& vulkanContext)
    : CEngineLoop(system, window, vulkanContext)
    , mSceneHandler(*this, mComponentManager, mFontHandler, mMeshManager,
                    system, mGameObjectManager)
    , mSceneLoader(*this, system.GetFileHandler(), mTextureManager,
                   mSpriteManager, mComponentManager, mFontHandler,
                   mMeshManager, system, mGameObjectManager)
    , mToolHandler(mComponentManager, system.GetFileHandler(), mSceneHandler,
                   mFontHandler, mBufferHandler, mTextureManager, mSceneLoader,
                   mCameraManager, mGameObjectManager) {

    mOverlordManager.CreateOverlord(window);
    mToolHandler.Initialize();
    mFontHandler.LoadAllThePolices();
    auto filePath = std::format("{}/common/textures/bricks",
                                system.GetFileHandler().GetAssetsFolder());
    mSpriteManager.LoadSpriteSheet(filePath);
}

void CGameLoop::GameLoop(float deltaTime) {
    // Game-specific update logic goes here
}
} // namespace Core
