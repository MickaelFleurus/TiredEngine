
#pragma once
#include <vulkan/vulkan.h>

struct SDL_Window;
union SDL_Event;

namespace Debug {
class IOverlord {
public:
    virtual ~IOverlord() = default;

    virtual void Initialize(SDL_Window* window) = 0;
    virtual bool PrepareRender(SDL_Window* window) = 0;
    virtual void Render(VkCommandBuffer cmd, VkPipeline pipeline) = 0;
    virtual void HandleEvent(const SDL_Event* e) = 0;
};
} // namespace Debug
