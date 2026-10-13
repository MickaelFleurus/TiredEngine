#include "core/GameLoop.h"

#include "engine/system/System.h"

namespace Core {
CGameLoop::CGameLoop(System::CSystem& system, Vulkan::SContext& context,
                     Vulkan::CSwapchain& swapchain)
    : CEngineLoop(system, context, swapchain)
    , mSceneHandler(*this, system)
    , mSceneLoader(*this, system.GetFileHandler(), mEntityManager,
                   mComponentManager, system) {

    mOverlordManager.CreateOverlord(context.window.get());
    mSceneLoader.LoadSceneFromFile(std::format(
        "{}/scene/debug.json", system.GetFileHandler().GetAssetsFolder()));

    mFontHandler.LoadAllThePolices();
}

void CGameLoop::GameLoop(float deltaTime) {
    // Game-specific update logic goes here
}
} // namespace Core
