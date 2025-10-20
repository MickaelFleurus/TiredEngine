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
        : mSDLWindow(std::unique_ptr<SDL_Window, decltype(kSDLWindowDeleter)>(
              SDL_CreateWindow("SDL3 window", kWindowWidth, kWindowHeight,
                               SDL_WINDOW_RESIZABLE),
              kSDLWindowDeleter))
        , mDevice(
              std::unique_ptr<SDL_GPUDevice, decltype(kSDLGPUDeviceDeleter)>(
                  SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV |
                                          SDL_GPU_SHADERFORMAT_MSL |
                                          SDL_GPU_SHADERFORMAT_DXIL,
                                      true, nullptr),
                  kSDLGPUDeviceDeleter)) {

        SDL_InitSubSystem(SDL_INIT_VIDEO);
        SDL_ClaimWindowForGPUDevice(mDevice.get(), mSDLWindow.get());
    }

    ~CImpl() {
        EndRender();
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    void Render() {
        if (mRenderPass) {
        }
    }

    SDL_GPUDevice& GetDevice() const {
        return *mDevice;
    }

    SDL_Window& GetWindow() const {
        return *mSDLWindow;
    }

    bool PrepareRender() {
        mCommandBuffer = SDL_AcquireGPUCommandBuffer(mDevice.get());
        if (!mCommandBuffer) {
            SDL_Log("Failed to acquire command buffer: %s", SDL_GetError());
            return false;
        }
        return true;
    }

    bool BeginRender() {
        // Get swapchain texture
        SDL_GPUTexture* swapchainTexture;
        if (!SDL_AcquireGPUSwapchainTexture(mCommandBuffer, mSDLWindow.get(),
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
        mRenderPass =
            SDL_BeginGPURenderPass(mCommandBuffer, &colorTarget, 1, nullptr);
        if (!mRenderPass) {
            SDL_Log("Failed to begin render pass: %s", SDL_GetError());
            return false;
        }

        return true;
    }

    void EndRender() {
        if (mRenderPass) {
            SDL_EndGPURenderPass(mRenderPass);
            mRenderPass = nullptr;
        }

        if (mCommandBuffer) {
            SDL_SubmitGPUCommandBuffer(mCommandBuffer);
            mCommandBuffer = nullptr;
        }
    }

    SDL_GPUCommandBuffer& GetCommandBuffer() const {
        return *mCommandBuffer;
    }

    SDL_GPURenderPass& GetRenderPass() const {
        return *mRenderPass;
    }

private:
    std::unique_ptr<SDL_Window, decltype(kSDLWindowDeleter)> mSDLWindow;
    std::unique_ptr<SDL_GPUDevice, decltype(kSDLGPUDeviceDeleter)> mDevice;
    SDL_GPUCommandBuffer* mCommandBuffer = nullptr;
    SDL_GPURenderPass* mRenderPass = nullptr;
};

CWindow::CWindow() : mImpl(std::make_unique<CWindow::CImpl>()) {
}

CWindow::~CWindow() = default;

bool CWindow::PrepareRender() {
    return mImpl->PrepareRender();
}

bool CWindow::BeginRender() {
    return mImpl->BeginRender();
}

void CWindow::Render() {
    mImpl->Render();
}

void CWindow::EndRender() {
    mImpl->EndRender();
}

SDL_GPUDevice& CWindow::GetDevice() const {
    return mImpl->GetDevice();
}

SDL_Window& CWindow::Get() const {
    return mImpl->GetWindow();
}

SDL_GPUCommandBuffer& CWindow::GetCommandBuffer() const {
    return mImpl->GetCommandBuffer();
}

SDL_GPURenderPass& CWindow::GetRenderPass() const {
    return mImpl->GetRenderPass();
}

} // namespace Renderer