#pragma once
#include "engine/debug/IOverlord.h"
#include "engine/utils/GuardedContainer.h"

class CToken;

struct SDL_Window;
struct SDL_GPUDevice;

namespace Debug {

class IOverlordItem;

class COverlord : public IOverlord {
public:
    COverlord(SDL_Window& window, SDL_GPUDevice& device);
    ~COverlord();

    static void AddWidget(IOverlordItem& item, CToken& token);
    static void AddMenu(IOverlordItem& item, CToken& token);

    void PrepareRender(SDL_GPUCommandBuffer& cmd) override;
    void Render(SDL_GPUCommandBuffer& cmd, SDL_GPURenderPass& pass) override;
    void HandleEvent(const SDL_Event* e) override;

private:
    void RenderMenuBar();
    void RenderWidgets();

    static CGuardedContainer<IOverlordItem> mWidgets;
    static CGuardedContainer<IOverlordItem> mMenus;

    SDL_Window& mWindow;
    SDL_GPUDevice& mDevice;
};
} // namespace Debug