#pragma once

#include "debug/OverlordToolHandler.h"
#include "scene/SceneHandler.h"

#include <engine/core/EngineLoop.h>

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
    Debug::COverlordToolHandler mToolHandler;
};
} // namespace Core