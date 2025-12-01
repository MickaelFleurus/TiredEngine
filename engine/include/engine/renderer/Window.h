#pragma once
#include "engine/renderer/RendererUtils.h"

#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

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
class CVulkanContext;
class CVulkanRendering;
class CDescriptorStorage;
class CBufferHandler;
} // namespace Vulkan

namespace Material {
class CMaterialManager;
} // namespace Material

namespace Renderer {

class CRendererManager;

class CWindow {
public:
    CWindow(System::CSystem& system, SDL_Window* window,
            Vulkan::CVulkanContext& vulkanContext,
            Vulkan::CVulkanRendering& renderer,
            Vulkan::CBufferHandler& bufferHandler,
            Material::CMaterialManager& materialManager,
            Vulkan::CDescriptorStorage& descriptorStorage,
            CRendererManager& rendererManager);
    ~CWindow();

    bool BeginRender();
    void Render(Scene::CAbstractScene& scene,
                Component::CComponentManager& componentManager);
    void EndRender();

    std::optional<uint32_t> GetImageIndex() const {
        return mImageIndex;
    }
    SDL_Window* GetSDLWindow() const {
        return mSDLWindow;
    }

private:
    const System::CSystem& mSystem;
    SDL_Window* mSDLWindow;
    Vulkan::CVulkanContext& mVulkanContext;
    Vulkan::CVulkanRendering& mRenderer;
    Vulkan::CBufferHandler& mBufferHandler;
    Vulkan::CDescriptorStorage& mDescriptorStorage;
    Material::CMaterialManager& mMaterialManager;
    CRendererManager& mRendererManager;

    std::optional<uint32_t> mImageIndex = std::nullopt;
    std::vector<Renderer::SRenderable> mRenderables;

    VkViewport mViewport;
    VkRect2D mScissor;
};
} // namespace Renderer