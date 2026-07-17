#pragma once

#include <chrono>

#include "engine/component/ComponentManager.h"
#include "engine/core/CameraManager.h"
#include "engine/core/GameObjectManager.h"
#include "engine/core/Inputs.h"
#include "engine/core/MeshFactory.h"
#include "engine/core/MeshManager.h"
#include "engine/debug/OverlordManager.h"
#include "engine/font/FontHandler.h"
#include "engine/input/InputHandler.h"
#include "engine/material/MaterialFactory.h"
#include "engine/material/MaterialManager.h"
#include "engine/renderer/MemoryAllocator.h"
#include "engine/renderer/RendererManager.h"
#include "engine/renderer/SpriteManager.h"
#include "engine/renderer/TextureManager.h"
#include "engine/renderer/TransformManager.h"
#include "engine/renderer/Window.h"
#include "engine/thread/Pool.h"
#include "engine/vulkan/BufferHandler.h"
#include "engine/vulkan/DescriptorStorage.h"
#include "engine/vulkan/VulkanRendering.h"

namespace Vulkan {
struct SContext;
} // namespace Vulkan

namespace Scene {
class CAbstractScene;
class ISceneHandler;
} // namespace Scene

namespace System {
class CSystem;
}

class CAssetParser;

namespace Core {
class CEngineLoop {
public:
    CEngineLoop(System::CSystem& system, Vulkan::SContext& context,
                Vulkan::CSwapchain& swapchain);
    virtual ~CEngineLoop();

    void SetPendingScene(std::unique_ptr<Scene::CAbstractScene>&& scene);
    Scene::CAbstractScene* GetCurrentScene() const;

    bool Run();

    virtual void GameLoop(float deltaTime) = 0;

protected:
    Thread::CPool mThreadPool;
    Vulkan::SContext& mContext;
    Vulkan::CSwapchain& mSwapchain;
    Vulkan::CVulkanRendering mVulkanRendering;
    CAssetParser& mAssetParser;
    Core::CGameObjectManager mGameObjectManager;
    Core::CCameraManager mCameraManager;
    Vulkan::CDescriptorStorage mDescriptorStorage;
    Renderer::CMemoryAllocator mMemoryAllocator;
    Vulkan::CBufferHandler mBufferHandler;

    Material::CMaterialManager mMaterialManager;
    Renderer::CSpriteManager mSpriteManager;

    Core::CMeshManager mMeshManager;

    Renderer::CTextureManager mTextureManager;
    Font::CFontHandler mFontHandler;
    Component::CComponentManager mComponentManager;
    Renderer::CRendererManager mRendererManager;
    Renderer::CWindow mWindow;

    Debug::COverlordManager mOverlordManager;
    CInputs mInputs;
    Input::CInputHandler mInputHandler;
    std::unique_ptr<Scene::CAbstractScene> mCurrentScene;
    std::unique_ptr<Scene::CAbstractScene> mPendingScene;
    std::chrono::time_point<std::chrono::high_resolution_clock> mLastFrameTime;
};
} // namespace Core
