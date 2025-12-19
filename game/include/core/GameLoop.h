#pragma once

#include <engine/core/EngineLoop.h>

#include "debug/OverlordToolHandler.h"
#include "scene/GameSceneLoader.h"
#include "scene/SceneHandler.h"

namespace Vulkan {
class CVulkanContext;
} // namespace Vulkan

namespace Core {
class CGameLoop : public CEngineLoop {
public:
    CGameLoop(System::CSystem& system, SDL_Window* window,
              Vulkan::CVulkanContext& vulkanContext);
    void GameLoop(float deltaTime) override;

private:
    Scene::CSceneHandler mSceneHandler;
    Scene::CGameSceneLoader mSceneLoader;
    Debug::COverlordToolHandler mToolHandler;
};
} // namespace Core