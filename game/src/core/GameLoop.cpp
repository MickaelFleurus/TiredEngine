#include "core/GameLoop.h"
#include "scene/SceneHandler.h"

#include <magic_enum/magic_enum.hpp>

namespace {
constexpr const char* kGameName = "BreakoutExclamationMark!";
}

namespace Core {
CGameLoop::CGameLoop()
    : CEngineLoop(kGameName)
    , mSceneHandler(*this, mComponentManager, mWindowData)
    , mToolHandler(mComponentManager, mFileHandler, mSceneHandler) {
    mOverlordManager.CreateOverlord();
    mToolHandler.RegisterTools();
}

void CGameLoop::GameLoop(float deltaTime) {
    // Game-specific update logic goes here
}
} // namespace Core