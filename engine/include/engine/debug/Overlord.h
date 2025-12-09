#pragma once
#include "engine/debug/IOverlord.h"
#include "engine/utils/GuardedContainer.h"

class CToken;

struct SDL_Window;

namespace Vulkan {
class CVulkanContext;
class CVulkanRendering;
} // namespace Vulkan

namespace Debug {

class IOverlordItem;

class COverlord : public IOverlord {
public:
    COverlord(const Vulkan::CVulkanContext& context,
              const Vulkan::CVulkanRendering& rendering);
    ~COverlord();

    void Initialize(SDL_Window* window) override;

    static void AddWidget(IOverlordItem& item, CToken& token);
    static void AddMenu(IOverlordItem& item, CToken& token);

    bool PrepareRender(SDL_Window* window) override;
    void Render(VkCommandBuffer cmd, VkPipeline pipeline) override;
    void HandleEvent(const SDL_Event* e) override;

private:
    void RenderMenuBar();
    void RenderWidgets();

    static CGuardedContainer<IOverlordItem> mWidgets;
    static CGuardedContainer<IOverlordItem> mMenus;

    const Vulkan::CVulkanContext& mContext;
    const Vulkan::CVulkanRendering& mRendering;

    VkDescriptorPool mImguiPool = VK_NULL_HANDLE;

    float mFrameTime = 0.0f;
    int mFPS = 0;
    float mFPSUpdateTime = 0.0f;
};
} // namespace Debug