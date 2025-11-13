#pragma once
#include "engine/debug/IOverlord.h"
#include "engine/utils/GuardedContainer.h"

class CToken;

namespace Renderer {
class CWindow;
}

namespace Debug {

class IOverlordItem;

class COverlord : public IOverlord {
public:
    COverlord(const Renderer::CWindow& window);
    ~COverlord();

    void Initialize() override;

    static void AddWidget(IOverlordItem& item, CToken& token);
    static void AddMenu(IOverlordItem& item, CToken& token);

    bool PrepareRender() override;
    void Render(VkCommandBuffer cmd, VkPipeline pipeline) override;
    void HandleEvent(const SDL_Event* e) override;

private:
    void RenderMenuBar();
    void RenderWidgets();

    static CGuardedContainer<IOverlordItem> mWidgets;
    static CGuardedContainer<IOverlordItem> mMenus;

    const Renderer::CWindow& mWindow;

    VkDescriptorPool mImguiPool = VK_NULL_HANDLE;
};
} // namespace Debug