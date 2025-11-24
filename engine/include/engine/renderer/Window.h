#pragma once
struct SDL_Window;

namespace Scene {
class CAbstractScene;
} // namespace Scene

namespace Component {
class CComponentManager;
} // namespace Component

namespace System {
class CSystem;
} // namespace System

namespace Vulkan {
class IVulkanContextGetter;
class CVulkanRendering;
} // namespace Vulkan

namespace Renderer {

class CWindow {
public:
    CWindow(System::CSystem& system, SDL_Window* window,
            Vulkan::IVulkanContextGetter& vulkanContext,
            Vulkan::CVulkanRendering& renderer);
    ~CWindow();

    bool BeginRender();
    void Render(Scene::CAbstractScene& scene,
                Component::CComponentManager& componentManager);
    void EndRender();

private:
    const System::CSystem& mSystem;
    SDL_Window* mSDLWindow;
    Vulkan::IVulkanContextGetter& mVulkanContext;
    Vulkan::CVulkanRendering& mRenderer;

    std::optional<uint32_t> mImageIndex = std::nullopt;

    VkViewport mViewport;
    VkRect2D mScissor;
};
} // namespace Renderer