#include "engine/core/EngineLoop.h"
#include "engine/scene/AbstractScene.h"
#include "engine/system/System.h"

#include "engine/utils/Logger.h"
#include <SDL3/SDL.h>

namespace Core {

CEngineLoop::CEngineLoop(System::CSystem& system)
    : mWindow(system)
    , mOverlordManager(mWindow)
    , mInputs(mOverlordManager)
    , mLastFrameTime(std::chrono::high_resolution_clock::now())
    , mTextureManager(mWindow, system.GetFileHandler())
    , mMaterialFactory(mTextureManager, system.GetFileHandler(), mWindow)
    , mFontHandler(mTextureManager, system.GetFileHandler(), mMaterialFactory)
    , mComponentManager(mFontHandler, mWindow) {
}

CEngineLoop::~CEngineLoop() = default;

std::expected<void, const char*> CEngineLoop::Initialize() {
    if (!mWindow.Initialize()) {
        const char* error = SDL_GetError();
        if (error && *error) {
            return std::unexpected(error);
        }
    }
    return {};
}

void CEngineLoop::SetCurrentScene(
    std::unique_ptr<Scene::CAbstractScene>&& scene) {
    mCurrentScene.swap(scene);
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
        if (!mWindow.PrepareRender()) {
            LOG_ERROR("Failed to prepare render");
            continue;
        }
        mOverlordManager.PrepareRender(mWindow.GetCommandBuffer());

        if (mWindow.BeginRender()) {
            if (mCurrentScene) {
                mWindow.Render(*mCurrentScene, mComponentManager);
            }
            mOverlordManager.Render(mWindow.GetCommandBuffer(),
                                    mWindow.GetRenderPass());

            mWindow.EndRender();
        }
        mInputHandler.Swap();
    }
    return true;
}

} // namespace Core