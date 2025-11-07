#include "engine/core/EngineLoop.h"
#include "engine/scene/AbstractScene.h"

#include <SDL3/SDL.h>

namespace Core {

CEngineLoop::CEngineLoop(const char* gameName)
    : mWindowData(mWindow.GetWidth(), mWindow.GetHeight())
    , mFileHandler(gameName)
    , mOverlordManager(mWindow.Get(), mWindow.GetDevice())
    , mShaderFactory(mWindow.GetDevice())
    , mInputs(mOverlordManager)
    , mLastFrameTime(std::chrono::high_resolution_clock::now())
    , mTextureManager(&mWindow.GetDevice(), mFileHandler)
    , mMaterialFactory(mTextureManager, mFileHandler, mWindow.GetDevice(),
                       mWindow.Get())
    , mFontHandler(mTextureManager, mFileHandler, mMaterialFactory)
    , mComponentManager(mFontHandler, mWindow.GetTextRenderer()) {
}

CEngineLoop::~CEngineLoop() = default;

std::expected<void, const char*>
CEngineLoop::EverythingInitialisedCorrectly() const {
    const char* error = SDL_GetError();
    if (error && *error) {
        return std::unexpected(error);
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
            SDL_Log("Failed to prepare render");
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