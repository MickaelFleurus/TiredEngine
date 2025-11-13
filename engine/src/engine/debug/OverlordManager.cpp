#include "engine/debug/OverlordManager.h"

#include "engine/debug/Overlord.h"

namespace Debug {
COverlordManager::COverlordManager(const Renderer::CWindow& window)
    : mWindow(window) {
}

COverlordManager::~COverlordManager() = default;

bool COverlordManager::PrepareRender() {
    if (mOverlordImpl) {
        return mOverlordImpl->PrepareRender();
    }
    return false;
}

void COverlordManager::Render(VkCommandBuffer cmd, VkPipeline pipeline) {
    if (mOverlordImpl) {
        mOverlordImpl->Render(cmd, pipeline);
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