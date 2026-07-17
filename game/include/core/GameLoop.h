#pragma once

#include <engine/core/EngineLoop.h>

#include "debug/OverlordToolHandler.h"
#include "scene/GameSceneLoader.h"
#include "scene/SceneHandler.h"

namespace Vulkan {
struct SContext;
class CSwapchain;
} // namespace Vulkan

namespace Core {
class CGameLoop : public CEngineLoop {
public:
    CGameLoop(System::CSystem& system, Vulkan::SContext& context,
              Vulkan::CSwapchain& swapchain);
    void GameLoop(float deltaTime) override;

private:
    Scene::CSceneHandler mSceneHandler;
    Scene::CGameSceneLoader mSceneLoader;
    Debug::COverlordToolHandler mToolHandler;
};
} // namespace Core
