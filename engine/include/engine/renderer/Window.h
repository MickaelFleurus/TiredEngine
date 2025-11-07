#pragma once
#include <memory>

struct SDL_GPUDevice;
struct SDL_Window;
struct SDL_GPUCommandBuffer;
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

class CWindow {
public:
    CWindow(System::CSystem& system);
    ~CWindow();

    bool Initialize();

    bool PrepareRender();
    bool BeginRender();
    void Render(Scene::CAbstractScene& scene,
                Component::CComponentManager& componentManager);
    void EndRender();
    SDL_GPUDevice* GetDevice() const;
    SDL_Window* Get() const;

    SDL_GPUCommandBuffer* GetCommandBuffer() const;
    SDL_GPURenderPass* GetRenderPass() const;

    CTextRenderer& GetTextRenderer();

private:
    class CImpl;
    std::unique_ptr<CImpl> mImpl;
    const System::CSystem& mSystem;
};
} // namespace Renderer