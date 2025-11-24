#include "core/GameLoop.h"
#include "engine/system/System.h"
#include "engine/utils/Asserts.h"

namespace Core {
CGameLoop::CGameLoop(System::CSystem& system, SDL_Window* window,
                     Vulkan::CVulkanContext& vulkanContext)
    : CEngineLoop(system)
    , mSceneHandler(*this, mComponentManager, mFontHandler, system)
    , mToolHandler(mComponentManager, system.GetFileHandler(), mSceneHandler,
                   mFontHandler) {
}

std::expected<void, const char*> CGameLoop::Initialize() {
    if (auto initExpected = CEngineLoop::Initialize(); !initExpected) {
        return initExpected;
    }

    mOverlordManager.CreateOverlord();
    mToolHandler.Initialize();

    return {};
}

void CGameLoop::GameLoop(float deltaTime) {
    // Game-specific update logic goes here
}
} // namespace Core