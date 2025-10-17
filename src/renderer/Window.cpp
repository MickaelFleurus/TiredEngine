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
        EndRender();
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    void Render() {
        if (m_RenderPass) {
        }
    }

    SDL_GPUDevice& GetDevice() const {
        return *m_Device;
    }

    SDL_Window& GetWindow() const {
        return *m_SDLWindow;
    }

    bool PrepareRender() {
        m_CommandBuffer = SDL_AcquireGPUCommandBuffer(m_Device.get());
        if (!m_CommandBuffer) {
            SDL_Log("Failed to acquire command buffer: %s", SDL_GetError());
            return false;
        }
        return true;
    }

    bool BeginRender() {
        // Get swapchain texture
        SDL_GPUTexture* swapchainTexture;
        if (!SDL_AcquireGPUSwapchainTexture(m_CommandBuffer, m_SDLWindow.get(),
                                            &swapchainTexture, nullptr,
                                            nullptr)) {
            SDL_Log("Failed to acquire swapchain texture: %s", SDL_GetError());
            return false;
        }

        if (!swapchainTexture) {
            SDL_Log("Swapchain texture is null");
            return false;
        }

        // Set up color target
        SDL_GPUColorTargetInfo colorTarget = {};
        colorTarget.texture = swapchainTexture;
        colorTarget.clear_color = {0.0f, 0.0f, 0.0f, 1.0f}; // Black background
        colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTarget.store_op = SDL_GPU_STOREOP_STORE;

        // Begin render pass
        m_RenderPass =
            SDL_BeginGPURenderPass(m_CommandBuffer, &colorTarget, 1, nullptr);
        if (!m_RenderPass) {
            SDL_Log("Failed to begin render pass: %s", SDL_GetError());
            return false;
        }

        return true;
    }

    void EndRender() {
        if (m_RenderPass) {
            SDL_EndGPURenderPass(m_RenderPass);
            m_RenderPass = nullptr;
        }

        if (m_CommandBuffer) {
            SDL_SubmitGPUCommandBuffer(m_CommandBuffer);
            m_CommandBuffer = nullptr;
        }
    }

    SDL_GPUCommandBuffer& GetCommandBuffer() const {
        return *m_CommandBuffer;
    }

    SDL_GPURenderPass& GetRenderPass() const {
        return *m_RenderPass;
    }

private:
    std::unique_ptr<SDL_Window, decltype(kSDLWindowDeleter)> m_SDLWindow;
    std::unique_ptr<SDL_GPUDevice, decltype(kSDLGPUDeviceDeleter)> m_Device;
    SDL_GPUCommandBuffer* m_CommandBuffer = nullptr;
    SDL_GPURenderPass* m_RenderPass = nullptr;
};

CWindow::CWindow() : m_Impl(std::make_unique<CWindow::CImpl>()) {
}

CWindow::~CWindow() = default;

bool CWindow::PrepareRender() {
    return m_Impl->PrepareRender();
}

bool CWindow::BeginRender() {
    return m_Impl->BeginRender();
}

void CWindow::Render() {
    m_Impl->Render();
}

void CWindow::EndRender() {
    m_Impl->EndRender();
}

SDL_GPUDevice& CWindow::GetDevice() const {
    return m_Impl->GetDevice();
}

SDL_Window& CWindow::Get() const {
    return m_Impl->GetWindow();
}

SDL_GPUCommandBuffer& CWindow::GetCommandBuffer() const {
    return m_Impl->GetCommandBuffer();
}

SDL_GPURenderPass& CWindow::GetRenderPass() const {
    return m_Impl->GetRenderPass();
}

} // namespace Renderer