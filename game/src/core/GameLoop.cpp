#include "core/GameLoop.h"

#include "engine/system/System.h"
#include "engine/utils/Asserts.h"

namespace Core {
CGameLoop::CGameLoop(System::CSystem& system, SDL_Window* window,
                     Vulkan::CVulkanContext& vulkanContext)
    : CEngineLoop(system, window, vulkanContext)
    , mSceneHandler(*this, mComponentManager, mFontHandler, mMeshManager,
                    system)
    , mSceneLoader(*this, system.GetFileHandler(), mTextureManager,
                   mSpriteManager, mComponentManager, mFontHandler,
                   mMeshManager, system)
    , mToolHandler(mComponentManager, system.GetFileHandler(), mSceneHandler,
                   mFontHandler, mBufferHandler, mTextureManager,
                   mSceneLoader) {

    mOverlordManager.CreateOverlord(window);
    mToolHandler.Initialize();
    mFontHandler.LoadAllThePolices();
}

void CGameLoop::GameLoop(float deltaTime) {
    // Game-specific update logic goes here
}
} // namespace Core
