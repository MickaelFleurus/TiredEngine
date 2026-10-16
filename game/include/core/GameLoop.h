#pragma once

#include <engine/core/EngineLoop.h>

#include "engine/debug/BufferWidget.h"
#include "engine/debug/Camera3DWidget.h"
#include "engine/debug/CameraUIWidget.h"
#include "engine/debug/InputStateWatcher.h"
#include "engine/debug/LevelSceneLoaderWidget.h"
#include "engine/debug/SceneLoaderOverlord.h"
#include "engine/utils/Token.h"
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
    CToken mOverlordToken;
    Debug::CSceneLoaderOverlord mSceneMenu;
    Debug::CInputStateWatcher mInputStateWatcher;
    Debug::CCamera3DWidget mCamera3DWidget;
    Debug::CCameraUIWidget mCameraUIWidget;
    Debug::CLevelSceneLoaderWidget mLevelSceneLoaderWidget;
    Debug::CBufferWidget mBufferWidget;
};
} // namespace Core
