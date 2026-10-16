#pragma once
#include <optional>

#include <vulkan/vulkan.h>

#include "engine/vulkan/VulkanContext.h"

struct SDL_Window;

namespace Scene {
class CAbstractScene;
} // namespace Scene

namespace System {
class CSystem;
} // namespace System

namespace Vulkan {
class CSwapchain;
class CVulkanRendering;
class CBufferHandler;
} // namespace Vulkan

namespace Material {
class CMaterialManager;
} // namespace Material

namespace Core {
class CCameraManager;
} // namespace Core

namespace Renderer {

class CRendererManager;

class CWindow {
public:
    CWindow(System::CSystem& system, Vulkan::SContext& context,
            Vulkan::CSwapchain& swapchain, Vulkan::CVulkanRendering& renderer,
            Vulkan::CBufferHandler& bufferHandler,
            Core::CCameraManager& cameraManager);
    ~CWindow();

    bool BeginRender();
    void Render(Scene::CAbstractScene& scene);
    void EndRender();

    std::optional<uint32_t> GetImageIndex() const;
    SDL_Window* GetSDLWindow() const;

private:
    const System::CSystem& mSystem;
    SDL_Window* mSDLWindow;
    Vulkan::SContext& mContext;
    Vulkan::CSwapchain& mSwapchain;
    Vulkan::CVulkanRendering& mRenderer;
    Vulkan::CBufferHandler& mBufferHandler;
    Core::CCameraManager& mCameraManager;

    std::optional<uint32_t> mImageIndex = std::nullopt;

    VkViewport mViewport;
    VkRect2D mScissor;
};
} // namespace Renderer
