#include "core/GameLoop.h"
#include "scene/DebugScene.h"

namespace Core {
CGameLoop::CGameLoop() : CEngineLoop(), mToolHandler(mSceneLoader) {
    mOverlordManager.CreateOverlord();
    mToolHandler.RegisterTools();
}

void CGameLoop::GameLoop(float deltaTime) {
    // Game-specific update logic goes here
}
} // namespace Core