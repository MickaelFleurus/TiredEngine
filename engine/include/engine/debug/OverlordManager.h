#pragma once
#include <memory>
#include <vulkan/vulkan.h>
struct SDL_Window;
union SDL_Event;

namespace Vulkan {
class CVulkanRendering;
class CVulkanContext;
} // namespace Vulkan

namespace Debug {
class IOverlord;

class COverlordManager {
public:
    COverlordManager(const Vulkan::CVulkanContext& context,
                     const Vulkan::CVulkanRendering& rendering);
    ~COverlordManager();

    bool PrepareRender(SDL_Window* window);
    void Render(VkCommandBuffer cmd, VkPipeline pipeline = VK_NULL_HANDLE);

    void HandleEvent(const SDL_Event* e);

    void CreateOverlord(SDL_Window* window);

private:
    std::unique_ptr<IOverlord> mOverlordImpl;
    const Vulkan::CVulkanContext& mContext;
    const Vulkan::CVulkanRendering& mRendering;
};
} // namespace Debug