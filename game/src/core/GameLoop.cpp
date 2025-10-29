#include "core/GameLoop.h"
#include "scene/DebugScene.h"

namespace {
constexpr const char* kGameName = "BreakoutExclamationMark!";
}

namespace Core {
CGameLoop::CGameLoop()
    : CEngineLoop(kGameName)
    , mToolHandler(mComponentManager, mFileHandler, mSceneLoader) {
    mOverlordManager.CreateOverlord();
    mToolHandler.RegisterTools();
}

void CGameLoop::GameLoop(float deltaTime) {
    // Game-specific update logic goes here
}
} // namespace Core