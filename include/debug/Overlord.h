#pragma once
#include "utils/GuardedContainer.h"

class CToken;

class SDL_Window;
class SDL_GPUDevice;
class SDL_GPUCommandBuffer;
class SDL_GPURenderPass;
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
    static CGuardedContainer<IOverlordItem> m_Items;
    SDL_Window& m_Window;
    SDL_GPUDevice& m_Device;
};
} // namespace Debug