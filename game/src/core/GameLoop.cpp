#include "core/GameLoop.h"
#include "engine/system/System.h"
#include "engine/utils/Asserts.h"

namespace Core {
CGameLoop::CGameLoop(System::CSystem& system, SDL_Window* window,
                     Vulkan::CVulkanContext& vulkanContext)
    : CEngineLoop(system, window, vulkanContext)
    , mSceneHandler(*this, mComponentManager, mFontHandler, system)
    , mToolHandler(mComponentManager, system.GetFileHandler(), mSceneHandler,
                   mFontHandler) {

    mOverlordManager.CreateOverlord(window);
    mToolHandler.Initialize();
}

void CGameLoop::GameLoop(float deltaTime) {
    // Game-specific update logic goes here
}
} // namespace Core