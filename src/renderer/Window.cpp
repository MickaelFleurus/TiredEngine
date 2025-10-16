#include "renderer/Window.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_init.h"

#include "debug/Overlord.h"
#include <functional>

namespace {
constexpr int kWindowWidth = 1920;
constexpr int kWindowHeight = 1080;

constexpr auto kSDLWindowDeleter = [](SDL_Window* window) {
    if (window) {
        SDL_DestroyWindow(window);
    }
};

constexpr auto kSDLGPUDeviceDeleter = [](SDL_GPUDevice* device) {
    if (device) {
        SDL_DestroyGPUDevice(device);
    }
};

} // namespace

namespace Renderer {

class CWindow::CImpl {
public:
    CImpl()
        : m_SDLWindow(std::unique_ptr<SDL_Window, decltype(kSDLWindowDeleter)>(
              SDL_CreateWindow("SDL3 window", kWindowWidth, kWindowHeight,
                               SDL_WINDOW_RESIZABLE),
              kSDLWindowDeleter))
        , m_Device(
              std::unique_ptr<SDL_GPUDevice, decltype(kSDLGPUDeviceDeleter)>(
                  SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV |
                                          SDL_GPU_SHADERFORMAT_MSL |
                                          SDL_GPU_SHADERFORMAT_DXIL,
                                      true, nullptr),
                  kSDLGPUDeviceDeleter)) {

        SDL_InitSubSystem(SDL_INIT_VIDEO);
        SDL_ClaimWindowForGPUDevice(m_Device.get(), m_SDLWindow.get());
    }

    ~CImpl() {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    void Render() {
    }

    SDL_GPUDevice& GetDevice() const {
        return *m_Device;
    }

private:
    std::unique_ptr<SDL_Window, decltype(kSDLWindowDeleter)> m_SDLWindow;
    std::unique_ptr<SDL_GPUDevice, decltype(kSDLGPUDeviceDeleter)> m_Device;
};

CWindow::CWindow() : m_Impl(std::make_unique<CWindow::CImpl>()) {
}

CWindow::~CWindow() = default;

void CWindow::Render() {
    m_Impl->Render();
}

SDL_GPUDevice& CWindow::GetDevice() const {
    return m_Impl->GetDevice();
}

} // namespace Renderer