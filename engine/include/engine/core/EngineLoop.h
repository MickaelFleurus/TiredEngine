#pragma once

#include "engine/core/Inputs.h"
#include "engine/debug/OverlordManager.h"
#include "engine/input/InputHandler.h"

#include "engine/renderer/BufferHandler.h"
#include "engine/renderer/DescriptorStorage.h"
#include "engine/renderer/MemoryAllocator.h"
#include "engine/renderer/Window.h"

#include "engine/component/ComponentManager.h"
#include "engine/material/MaterialFactory.h"

#include "engine/font/FontHandler.h"
#include "engine/utils/FileHandler.h"

#include "engine/renderer/TextRenderer.h"
#include "engine/renderer/TextureManager.h"
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

    Renderer::CDescriptorStorage mDescriptorStorage;
    Renderer::CMemoryAllocator mMemoryAllocator;
    Renderer::CBufferHandler mBufferHandler;
    Renderer::CTextRenderer mTextRenderer;
    Material::CMaterialFactory mMaterialFactory;
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