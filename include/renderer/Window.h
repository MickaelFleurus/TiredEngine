#pragma once
#include <memory>

class SDL_GPUDevice;

namespace Renderer {
class CWindow {
public:
    CWindow();
    ~CWindow();

    void Render();
    SDL_GPUDevice& GetDevice() const;

private:
    class CImpl;
    std::unique_ptr<CImpl> m_Impl;
};
} // namespace Renderer