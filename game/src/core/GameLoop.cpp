#include "core/GameLoop.h"
#include "scene/DebugScene.h"

namespace Core {
CGameLoop::CGameLoop() : CEngineLoop(), mToolHandler(*this) {
    mOverlordManager.CreateOverlord();
    mToolHandler.RegisterTools();
}

void CGameLoop::LoadScene(Scene::ESceneType sceneType) {
    StartScene(
        std::make_unique<Scene::DebugScene>(mComponentManager, mWindowData));
}

void CGameLoop::GameLoop(float deltaTime) {
    // Game-specific update logic goes here
}
} // namespace Core