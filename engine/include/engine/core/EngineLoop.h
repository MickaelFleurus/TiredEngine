#pragma once

#include "engine/component/ComponentManager.h"
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
#include "engine/renderer/TextureManager.h"
#include "engine/renderer/Window.h"
#include "engine/utils/FileHandler.h"
#include "engine/vulkan/BufferHandler.h"
#include "engine/vulkan/DescriptorStorage.h"
#include "engine/vulkan/VulkanRendering.h"

#include <chrono>

struct SDL_Window;

namespace Vulkan {
class CVulkanContext;
} // namespace Vulkan

namespace Scene {
class CAbstractScene;
class ISceneHandler;
} // namespace Scene

namespace System {
class CSystem;
}

namespace Core {
class CEngineLoop {
public:
    CEngineLoop(System::CSystem& system, SDL_Window* window,
                Vulkan::CVulkanContext& vulkanContext);
    virtual ~CEngineLoop();

    void SetPendingScene(std::unique_ptr<Scene::CAbstractScene>&& scene);
    Scene::CAbstractScene* GetCurrentScene() const;

    bool Run();

    virtual void GameLoop(float deltaTime) = 0;

protected:
    Vulkan::CVulkanContext& mVulkanContext;
    Vulkan::CVulkanRendering mVulkanRendering;

    Vulkan::CDescriptorStorage mDescriptorStorage;
    Renderer::CMemoryAllocator mMemoryAllocator;
    Vulkan::CBufferHandler mBufferHandler;

    Material::CMaterialManager mMaterialManager;
    Renderer::CRendererManager mRendererManager;

    Core::CMeshManager mMeshManager;

    Renderer::CWindow mWindow;
    Renderer::CTextureManager mTextureManager;
    Font::CFontHandler mFontHandler;
    Component::CComponentManager mComponentManager;

    Debug::COverlordManager mOverlordManager;
    CInputs mInputs;
    Input::CInputHandler mInputHandler;
    std::unique_ptr<Scene::CAbstractScene> mCurrentScene;
    std::unique_ptr<Scene::CAbstractScene> mPendingScene;
    std::chrono::time_point<std::chrono::high_resolution_clock> mLastFrameTime;
};
} // namespace Core