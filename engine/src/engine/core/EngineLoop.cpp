#include "engine/core/EngineLoop.h"
#include "engine/scene/AbstractScene.h"
#include "engine/system/System.h"

#include "engine/renderer/Window.h"
#include "engine/vulkan/VulkanContext.h"

#include "engine/utils/Logger.h"
#include <SDL3/SDL.h>

namespace Core {

CEngineLoop::CEngineLoop(System::CSystem& system, SDL_Window* window,
                         Vulkan::CVulkanContext& vulkanContext)
    : mVulkanContext(vulkanContext)
    , mVulkanRendering(vulkanContext)
    , mDescriptorStorage(mVulkanContext)
    , mMemoryAllocator(mVulkanContext)
    , mBufferHandler(mVulkanContext, mMemoryAllocator)
    , mTextRenderer(mBufferHandler.GetInstanceBufferHandle(),
                    mBufferHandler.GetInstancesInfoBufferHandle())
    , mMaterialFactory(mTextureManager, system.GetFileHandler(), mVulkanContext,
                       mDescriptorStorage)
    , mWindow(system, window, vulkanContext, mVulkanRendering, mBufferHandler,
              mTextRenderer, mMaterialFactory, mDescriptorStorage)
    , mTextureManager(mVulkanContext, mVulkanRendering, mMemoryAllocator,
                      mBufferHandler, system.GetFileHandler(),
                      mDescriptorStorage)
    , mFontHandler(mTextureManager, system.GetFileHandler(), mMaterialFactory,
                   mBufferHandler.GetVerticesBufferHandle(),
                   mBufferHandler.GetIndexesBufferHandle())
    , mComponentManager(mFontHandler, mTextRenderer)
    , mOverlordManager(mVulkanContext, mVulkanRendering)
    , mInputs(mOverlordManager)
    , mLastFrameTime(std::chrono::high_resolution_clock::now()) {
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

        mOverlordManager.PrepareRender(mWindow.GetSDLWindow());

        if (mWindow.BeginRender()) {
            if (mCurrentScene) {
                mWindow.Render(*mCurrentScene, mComponentManager);
            }
            mOverlordManager.Render(mVulkanContext.GetCommandBuffer(
                mWindow.GetImageIndex().value()));

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