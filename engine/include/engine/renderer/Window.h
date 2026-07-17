#pragma once
#include <optional>
#include <vector>

#include <vulkan/vulkan.h>

#include "engine/core/GameObjectId.h"
#include "engine/renderer/RendererUtils.h"

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
class CSwapchain;
class CVulkanRendering;
class CDescriptorStorage;
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
    CWindow(System::CSystem& system, SDL_Window* window,
            Vulkan::CSwapchain& swapchain, Vulkan::CVulkanRendering& renderer,
            Vulkan::CBufferHandler& bufferHandler,
            Material::CMaterialManager& materialManager,
            Vulkan::CDescriptorStorage& descriptorStorage,
            CRendererManager& rendererManager,
            Core::CCameraManager& cameraManager);
    ~CWindow();

    bool BeginRender();
    void Render(Scene::CAbstractScene& scene,
                Component::CComponentManager& componentManager);
    void EndRender();

    std::optional<uint32_t> GetImageIndex() const;
    SDL_Window* GetSDLWindow() const;

private:
    const System::CSystem& mSystem;
    SDL_Window* mSDLWindow;
    Vulkan::CSwapchain& mSwapchain;
    Vulkan::CVulkanRendering& mRenderer;
    Vulkan::CBufferHandler& mBufferHandler;
    Vulkan::CDescriptorStorage& mDescriptorStorage;
    Material::CMaterialManager& mMaterialManager;
    CRendererManager& mRendererManager;
    Core::CCameraManager& mCameraManager;

    std::optional<uint32_t> mImageIndex = std::nullopt;

    VkViewport mViewport;
    VkRect2D mScissor;
};
} // namespace Renderer
