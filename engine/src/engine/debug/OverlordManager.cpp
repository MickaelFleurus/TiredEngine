#include "engine/debug/OverlordManager.h"

#include "engine/debug/Overlord.h"

namespace Debug {
COverlordManager::COverlordManager(SDL_Window& window, SDL_GPUDevice& device)
    : mWindow(window), mDevice(device) {
}

COverlordManager::~COverlordManager() = default;

void COverlordManager::PrepareRender(SDL_GPUCommandBuffer& cmd) {
    if (mOverlordImpl) {
        mOverlordImpl->PrepareRender(cmd);
    }
}

void COverlordManager::Render(SDL_GPUCommandBuffer& cmd,
                              SDL_GPURenderPass& pass) {
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
    // TODO : Factory pattern for different Overlord implementations
    mOverlordImpl = std::make_unique<COverlord>(mWindow, mDevice);
}

} // namespace Debug