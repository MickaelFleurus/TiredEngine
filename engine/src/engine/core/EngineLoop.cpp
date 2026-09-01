#include "engine/core/EngineLoop.h"

#include <SDL3/SDL.h>

#include "engine/renderer/Window.h"
#include "engine/scene/AbstractScene.h"
#include "engine/system/System.h"
#include "engine/vulkan/VulkanContext.h"

namespace Core {

CEngineLoop::CEngineLoop(System::CSystem& system, Vulkan::SContext& context,
                         Vulkan::CSwapchain& swapchain)
    : mContext(context)
    , mSwapchain(swapchain)
    , mVulkanRendering(context, swapchain)
    , mAssetParser(system.GetAssetParser())
    , mCameraManager(system.GetFileHandler())
    , mBufferHandler(mContext)
    , mPipelineFactory(mContext, mSwapchain)
    , mTextureManager(context, swapchain, mVulkanRendering, mBufferHandler,
                      system.GetFileHandler(), mAssetParser)
    , mFontHandler(system.GetFileHandler(), mThreadPool, mAssetParser,
                   mTextureManager)

    , mWindow(system, context, swapchain, mVulkanRendering, mBufferHandler,
              mCameraManager)
    , mOverlordManager(context, swapchain, mVulkanRendering)
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
                mWindow.Render(*mCurrentScene);
            }
            mOverlordManager.Render(
                mSwapchain.GetCommandBuffer(mWindow.GetImageIndex().value()));

            mWindow.EndRender();
        }
        if (mPendingScene) {
            if (mCurrentScene) {
                mCurrentScene->Unload();
            }

            mPendingScene->Load();

            mCurrentScene.swap(mPendingScene);
            mPendingScene.reset();
        }
        mInputHandler.Swap();
        mAssetParser.CheckAndUpdateAssets();
    }
    return true;
}

} // namespace Core
