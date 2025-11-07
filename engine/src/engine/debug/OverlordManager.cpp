#include "engine/debug/OverlordManager.h"

#include "engine/debug/Overlord.h"

namespace Debug {
COverlordManager::COverlordManager(const Renderer::CWindow& window)
    : mWindow(window) {
}

COverlordManager::~COverlordManager() = default;

void COverlordManager::PrepareRender(SDL_GPUCommandBuffer* cmd) {
    if (mOverlordImpl) {
        mOverlordImpl->PrepareRender(cmd);
    }
}

void COverlordManager::Render(SDL_GPUCommandBuffer* cmd,
                              SDL_GPURenderPass* pass) {
    if (mOverlordImpl) {
        mOverlordImpl->Render(cmd, pass);
    }
}

void COverlordManager::HandleEvent(const SDL_Event* e) {
    if (mOverlordImpl) {
        mOverlordImpl->HandleEvent(e);
    }
}

void COverlordManager::CreateOverlord() {
    mOverlordImpl = std::make_unique<COverlord>(mWindow);
    mOverlordImpl->Initialize();
}

} // namespace Debug