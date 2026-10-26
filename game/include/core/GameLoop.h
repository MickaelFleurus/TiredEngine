#pragma once

#include <engine/core/EngineLoop.h>

#include "engine/debug/AssetExplorerWidget.h"
#include "engine/debug/BufferWidget.h"
#include "engine/debug/Camera3DWidget.h"
#include "engine/debug/CameraUIWidget.h"
#include "engine/debug/GameViewportWidget.h"
#include "engine/debug/InputStateWatcher.h"
#include "engine/debug/LevelSceneLoaderWidget.h"
#include "engine/debug/SceneHierarchy.h"
#include "engine/debug/SceneLoaderOverlord.h"
#include "engine/debug/SpriteAnimationWidget.h"
#include "engine/debug/SpritesWidget.h"
#include "engine/debug/TexturesWidget.h"
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
    Debug::SEntitySelection mEntitySelection;
    Debug::CSceneHierarchy mSceneHierarchy;
    Debug::CEntityInspector mEntityInspector;
    Debug::CTexturesWidget mTexturesWidget;
    Debug::CSpritesWidget mSpritesWidget;
    Debug::CSpriteAnimationWidget mSpriteAnimationWidget;
    Debug::CGameViewportWidget mGameViewportWidget;
    Debug::CAssetExplorerWidget mAssetExplorerWidget;
};
} // namespace Core
