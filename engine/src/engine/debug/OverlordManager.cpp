#include "engine/debug/OverlordManager.h"

#include "engine/debug/Overlord.h"

namespace Debug {
COverlordManager::COverlordManager(const Vulkan::SContext& context,
                                   Vulkan::CSwapchain& swapchain,
                                   const Vulkan::CVulkanRendering& rendering)
    : mContext(context), mSwapchain(swapchain), mRendering(rendering) {
}

COverlordManager::~COverlordManager() = default;

bool COverlordManager::PrepareRender(SDL_Window* window) {
    if (mOverlordImpl) {
        return mOverlordImpl->PrepareRender(window);
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

void COverlordManager::CreateOverlord(SDL_Window* window) {
    mOverlordImpl =
        std::make_unique<COverlord>(mContext, mSwapchain, mRendering);
    mOverlordImpl->Initialize(window);
}

} // namespace Debug
