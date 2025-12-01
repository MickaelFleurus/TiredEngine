#pragma once
#include <optional>
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
class CMaterialFactory;
} // namespace Material

namespace Renderer {

class CTextRenderer;

class CWindow {
public:
    CWindow(System::CSystem& system, SDL_Window* window,
            Vulkan::CVulkanContext& vulkanContext,
            Vulkan::CVulkanRendering& renderer,
            Vulkan::CBufferHandler& bufferHandler, CTextRenderer& textRenderer,
            Material::CMaterialFactory& materialFactory,
            Vulkan::CDescriptorStorage& descriptorStorage);
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
    CTextRenderer& mTextRenderer;
    Vulkan::CDescriptorStorage& mDescriptorStorage;
    Material::CMaterialFactory& mMaterialFactory;

    std::optional<uint32_t> mImageIndex = std::nullopt;

    VkViewport mViewport;
    VkRect2D mScissor;
};
} // namespace Renderer