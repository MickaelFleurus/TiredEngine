#include "engine/core/EngineLoop.h"
#include "engine/scene/AbstractScene.h"

#include <SDL3/SDL.h>

namespace Core {

CEngineLoop::CEngineLoop()
    : mOverlord(mWindow.Get(), mWindow.GetDevice())
    , mShaderFactory(mWindow.GetDevice())
    , mInputs(mOverlord)
    , mLastFrameTime(std::chrono::high_resolution_clock::now()) {
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

        mOverlord.PrepareRender(mWindow.GetCommandBuffer());
        if (mWindow.BeginRender()) {
            mOverlord.Render(mWindow.GetCommandBuffer(),
                             mWindow.GetRenderPass());
            mWindow.Render();
            mWindow.EndRender();
        }
        mInputHandler.Swap();
    }
    return false;
}
} // namespace Core