#pragma once
#include <memory>

struct SDL_GPUCommandBuffer;
struct SDL_GPURenderPass;
union SDL_Event;

namespace Renderer {
class CWindow;
}

namespace Debug {
class IOverlord;

class COverlordManager {
public:
    COverlordManager(const Renderer::CWindow& window);
    ~COverlordManager();

    void PrepareRender(SDL_GPUCommandBuffer* cmd);
    void Render(SDL_GPUCommandBuffer* cmd, SDL_GPURenderPass* pass);

    void HandleEvent(const SDL_Event* e);

    void CreateOverlord();

private:
    std::unique_ptr<IOverlord> mOverlordImpl;
    const Renderer::CWindow& mWindow;
};
} // namespace Debug