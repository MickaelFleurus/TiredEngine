#pragma once
#include <memory>

struct SDL_GPUDevice;
struct SDL_Window;
struct SDL_GPUCommandBuffer;
struct SDL_GPURenderPass;

namespace Renderer {
class CWindow {
public:
    CWindow();
    ~CWindow();

    bool PrepareRender();
    bool BeginRender();
    void Render();
    void EndRender();
    SDL_GPUDevice& GetDevice() const;
    SDL_Window& Get() const;

    SDL_GPUCommandBuffer& GetCommandBuffer() const;
    SDL_GPURenderPass& GetRenderPass() const;

private:
    class CImpl;
    std::unique_ptr<CImpl> mImpl;
};
} // namespace Renderer