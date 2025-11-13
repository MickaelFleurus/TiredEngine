#pragma once
#include <memory>
#include <vulkan/vulkan.h>

struct SDL_GPUDevice;
struct SDL_Window;
struct SDL_GPURenderPass;

namespace Scene {
class CAbstractScene;
} // namespace Scene

namespace Component {
class CComponentManager;
} // namespace Component

namespace System {
class CSystem;
} // namespace System

namespace Renderer {

class CTextRenderer;
class VulkanRenderer;

class CWindow {
public:
    CWindow(System::CSystem& system);
    ~CWindow();

    bool Initialize();

    bool BeginRender();
    void Render(Scene::CAbstractScene& scene,
                Component::CComponentManager& componentManager);
    void EndRender();

    SDL_GPUDevice* GetDevice() const {
        return nullptr;
    }
    SDL_Window* GetSDLWindow() const;

    SDL_GPURenderPass* GetRenderPass() const {
        return nullptr;
    }

    const VulkanRenderer& GetVulkanRenderer() const;

    CTextRenderer& GetTextRenderer();
    VkCommandBuffer GetCommandBuffer();

private:
    class CImpl;
    std::unique_ptr<CImpl> mImpl;
    const System::CSystem& mSystem;
};
} // namespace Renderer