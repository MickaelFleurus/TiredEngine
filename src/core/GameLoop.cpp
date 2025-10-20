#include "core/GameLoop.h"
#include "scene/AbstractScene.h"

#include <SDL3/SDL.h>

namespace Core {

CGameLoop::CGameLoop()
    : mOverlord(mWindow.Get(), mWindow.GetDevice())
    , mShaderFactory(mWindow.GetDevice())
    , mInputs(mOverlord) {
}

CGameLoop::~CGameLoop() = default;

std::expected<void, const char*>
CGameLoop::EverythingInitialisedCorrectly() const {
    const char* error = SDL_GetError();
    if (error && *error) {
        return std::unexpected(error);
    }
    return {};
}

bool CGameLoop::Run() {
    while (mInputs.Poll()) {
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