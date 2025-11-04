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

namespace Renderer {

class CTextRenderer;

class CWindow {
public:
    CWindow();
    ~CWindow();

    bool PrepareRender();
    bool BeginRender();
    void Render(Scene::CAbstractScene& scene,
                Component::CComponentManager& componentManager);
    void EndRender();
    SDL_GPUDevice& GetDevice() const;
    SDL_Window& Get() const;

    SDL_GPUCommandBuffer& GetCommandBuffer() const;
    SDL_GPURenderPass& GetRenderPass() const;

    CTextRenderer& GetTextRenderer();

    int GetWidth() const;
    int GetHeight() const;

private:
    class CImpl;
    std::unique_ptr<CImpl> mImpl;
};
} // namespace Renderer