#include "engine/core/EngineLoop.h"
#include "engine/scene/AbstractScene.h"
#include "engine/system/System.h"

#include "engine/renderer/Window.h"

#include "engine/utils/Logger.h"
#include <SDL3/SDL.h>

namespace Core {

CEngineLoop::CEngineLoop(System::CSystem& system, SDL_Window* window,
                         Vulkan::CVulkanContext& vulkanContext)
    : mVulkanContext(vulkanContext)
    , mVulkanRendering(vulkanContext)
    , mWindow(system, window, vulkanContext, mVulkanRendering)
    , mMemoryAllocator(mVulkanContext)
    , mBufferHandler(mVulkanContext, mMemoryAllocator)
    , mTextureManager(mWindow, system.GetFileHandler())
    , mFontHandler(mTextureManager, system.GetFileHandler(), mMaterialFactory,
                   mBufferHandler)
    , mComponentManager(mFontHandler, mWindow)
    , mOverlordManager(mVulkanContext, mVulkanRendering)
    , mInputs(mOverlordManager)
    , mLastFrameTime(std::chrono::high_resolution_clock::now())
    , mMaterialFactory(mTextureManager, system.GetFileHandler(), mWindow) {
}

CEngineLoop::~CEngineLoop() = default;

void CEngineLoop::SetPendingScene(
    std::unique_ptr<Scene::CAbstractScene>&& scene) {
    mPendingScene.swap(scene);
}

Scene::CAbstractScene* CEngineLoop::GetCurrentScene() const {
    return mCurrentScene.get();
}

bool CEngineLoop::Run() {
    while (mInputs.Poll()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration_cast<std::chrono::seconds>(
                              currentTime - mLastFrameTime)
                              .count();
        mLastFrameTime = currentTime;

        mInputHandler.Update();

        mOverlordManager.PrepareRender();

        if (mWindow.BeginRender()) {
            if (mCurrentScene) {
                mWindow.Render(*mCurrentScene, mComponentManager);
            }
            mOverlordManager.Render(mWindow.GetCommandBuffer());

            mWindow.EndRender();
        }
        if (mPendingScene) {
            mCurrentScene.swap(mPendingScene);
            mPendingScene.reset();
        }
        mInputHandler.Swap();
    }
    return true;
}

} // namespace Core