#pragma once
#include <memory>

class SDL_GPUDevice;
class SDL_Window;
class SDL_GPUCommandBuffer;
class SDL_GPURenderPass;

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
    std::unique_ptr<CImpl> m_Impl;
};
} // namespace Renderer