#pragma once
#include <memory>
#include <vulkan/vulkan.h>
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

    bool PrepareRender();
    void Render(VkCommandBuffer cmd, VkPipeline pipeline = VK_NULL_HANDLE);

    void HandleEvent(const SDL_Event* e);

    void CreateOverlord();

private:
    std::unique_ptr<IOverlord> mOverlordImpl;
    const Renderer::CWindow& mWindow;
};
} // namespace Debug