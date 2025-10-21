
#pragma once

struct SDL_GPUCommandBuffer;
struct SDL_GPURenderPass;
union SDL_Event;

namespace Debug {
class IOverlord {
public:
    virtual ~IOverlord() = default;

    virtual void PrepareRender(SDL_GPUCommandBuffer& cmd) = 0;
    virtual void Render(SDL_GPUCommandBuffer& cmd, SDL_GPURenderPass& pass) = 0;
    virtual void HandleEvent(const SDL_Event* e) = 0;
};
} // namespace Debug
