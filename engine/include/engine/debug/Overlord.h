#pragma once
#include "engine/debug/InputStateWatcher.h"
#include "engine/utils/GuardedContainer.h"

class CToken;

struct SDL_Window;
struct SDL_GPUDevice;
struct SDL_GPUCommandBuffer;
struct SDL_GPURenderPass;
union SDL_Event;

namespace Debug {

class IOverlordItem;

class COverlord {
public:
    COverlord(SDL_Window& window, SDL_GPUDevice& device);
    ~COverlord();

    static void AddMenu(IOverlordItem& item, CToken& token);
    void PrepareRender(SDL_GPUCommandBuffer& cmd);
    void Render(SDL_GPUCommandBuffer& cmd, SDL_GPURenderPass& pass);

    void HandleEvent(const SDL_Event* e);

private:
    void ShowMenuBar();

    static CGuardedContainer<IOverlordItem> mItems;
    SDL_Window& mWindow;
    SDL_GPUDevice& mDevice;
    CInputStateWatcher mInputStateWatcher;
};
} // namespace Debug