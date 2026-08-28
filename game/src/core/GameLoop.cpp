#include "core/GameLoop.h"

#include "engine/system/System.h"

namespace Core {
CGameLoop::CGameLoop(System::CSystem& system, Vulkan::SContext& context,
                     Vulkan::CSwapchain& swapchain)
    : CEngineLoop(system, context, swapchain)
    , mSceneHandler(*this, system)
    , mSceneLoader(*this, system.GetFileHandler(), system) {

    mOverlordManager.CreateOverlord(context.window.get());

    mFontHandler.LoadAllThePolices();
}

void CGameLoop::GameLoop(float deltaTime) {
    // Game-specific update logic goes here
}
} // namespace Core
