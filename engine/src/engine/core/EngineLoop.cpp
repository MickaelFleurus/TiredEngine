#include "engine/core/EngineLoop.h"

#include <SDL3/SDL.h>

#include "engine/renderer/Window.h"
#include "engine/scene/AbstractScene.h"
#include "engine/system/System.h"
#include "engine/utils/Logger.h"
#include "engine/vulkan/VulkanContext.h"

namespace Core {

CEngineLoop::CEngineLoop(System::CSystem& system, Vulkan::SContext& context,
                         Vulkan::CSwapchain& swapchain)
    : mContext(context)
    , mSwapchain(swapchain)
    , mVulkanRendering(context, swapchain)
    , mAssetParser(system.GetAssetParser())
    , mGameObjectManager()
    , mCameraManager(mGameObjectManager.GetTransformManager(),
                     system.GetFileHandler())
    , mBufferHandler(mContext)
    , mPipelineFactory(mContext, mSwapchain)
    , mMaterialManager(mTextureManager, system.GetFileHandler(),
                       mPipelineFactory)
    , mSpriteManager(system.GetAssetParser(), system.GetFileHandler(),
                     mTextureManager)
    , mTextureManager(context, swapchain, mVulkanRendering, mBufferHandler,
                      system.GetFileHandler(), system.GetAssetParser())
    , mFontHandler(mTextureManager, system.GetFileHandler(), mMaterialManager,
                   mThreadPool, mAssetParser)
    , mComponentManager(mFontHandler, mMaterialManager, mSpriteManager,
                        mCameraManager,
                        mGameObjectManager.GetTransformManager())
    , mRendererManager(mBufferHandler, mMaterialManager,
                       mGameObjectManager.GetTransformManager(),
                       mPipelineFactory)
    , mWindow(system, context.window.get(), swapchain, mVulkanRendering,
              mBufferHandler, mMaterialManager, mRendererManager,
              mCameraManager)
    , mOverlordManager(context, swapchain, mVulkanRendering)
    , mInputs(mOverlordManager)
    , mLastFrameTime(std::chrono::high_resolution_clock::now()) {
    mTextureManager.LoadTexture("WhitePixel");
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

        mGameObjectManager.Update();
        mComponentManager.Update(deltaTime);

        if (mWindow.BeginRender()) {
            if (mCurrentScene) {
                mWindow.Render(*mCurrentScene, mComponentManager);
            }
            mOverlordManager.Render(
                mSwapchain.GetCommandBuffer(mWindow.GetImageIndex().value()));

            mWindow.EndRender();
        }
        if (mPendingScene) {
            if (mCurrentScene) {
                mCurrentScene->Unload();
            }
            mRendererManager.FreeSceneData();

            mPendingScene->Load();
            mRendererManager.Prepare();

            mCurrentScene.swap(mPendingScene);
            mPendingScene.reset();
        }
        mInputHandler.Swap();
        mComponentManager.CleanDeleted();
        mAssetParser.CheckAndUpdateAssets();
    }
    return true;
}

} // namespace Core
