#pragma once
#include <memory>

struct SDL_Window;
struct SDL_GPUDevice;
struct SDL_GPUCommandBuffer;
struct SDL_GPURenderPass;
union SDL_Event;

namespace Debug {
class IOverlord;

class COverlordManager {
public:
    COverlordManager(SDL_Window& window, SDL_GPUDevice& device);
    ~COverlordManager();

    void PrepareRender(SDL_GPUCommandBuffer& cmd);
    void Render(SDL_GPUCommandBuffer& cmd, SDL_GPURenderPass& pass);

    void HandleEvent(const SDL_Event* e);

    void CreateOverlord();

private:
    std::unique_ptr<IOverlord> mOverlordImpl;
    SDL_Window& mWindow;
    SDL_GPUDevice& mDevice;
};
} // namespace Debug