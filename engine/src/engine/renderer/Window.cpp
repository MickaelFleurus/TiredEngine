#include "engine/renderer/Window.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include "engine/system/System.h"
#include "engine/utils/Logger.h"
#include "engine/vulkan/BufferHandler.h"
#include "engine/vulkan/VulkanRendering.h"
#include "engine/vulkan/VulkanSwapchain.h"

namespace {

VkViewport GetViewport(SDL_Window* window,
                       const System::SDisplayParameter& displayParams) {

    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(std::min(displayParams.width, w));
    viewport.height = static_cast<float>(std::min(displayParams.height, h));
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    return viewport;
}

VkRect2D GetScissor(VkViewport viewport) {
    VkRect2D scissor;
    scissor.offset = {0, 0};
    scissor.extent = {static_cast<uint32_t>(viewport.width),
                      static_cast<uint32_t>(viewport.height)};
    return scissor;
}

} // namespace

namespace Renderer {

CWindow::CWindow(System::CSystem& system, Vulkan::SContext& context,
                 Vulkan::CSwapchain& swapchain,
                 Vulkan::CVulkanRendering& renderer,
                 Vulkan::CBufferHandler& bufferHandler,
                 Core::CCameraManager& cameraManager)
    : mSystem(system)
    , mContext(context)
    , mSwapchain(swapchain)
    , mRenderer(renderer)
    , mBufferHandler(bufferHandler)
    , mCameraManager(cameraManager)
    , mSDLWindow(context.window.get())
    , mViewport(GetViewport(mSDLWindow, mSystem.GetDisplayParameters()))
    , mScissor(GetScissor(mViewport))

{
}

CWindow::~CWindow() {
    EndRender();
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void CWindow::Render(Scene::CAbstractScene& scene) {

    // mRendererManager.GenerateInstances(componentManager);
    // VkCommandBuffer commandBuffer =
    //     mSwapchain.GetCommandBuffer(mImageIndex.value());
    // mRendererManager.Render(commandBuffer, mContext.bindelessTextureSet,
    //                         mCameraManager.GetDefaultCamera3D(),
    //                         mCameraManager.GetCameraUI());
}

bool CWindow::BeginRender() {
    mImageIndex = mRenderer.AcquireNextImage();
    if (!mImageIndex.has_value()) {
        LOG_ERROR("Failed to acquire next swapchain image!");
        return false;
    }
    mRenderer.BeginRenderPass(mImageIndex.value(), mViewport, mScissor);
    return true;
}

void CWindow::EndRender() {
    if (mImageIndex.has_value()) {
        mRenderer.EndRenderPass(mImageIndex.value());

        mRenderer.SubmitAsync(mSwapchain.GetCommandBuffer(mImageIndex.value()),
                              mImageIndex.value());
        mRenderer.Present(mImageIndex.value());
        mRenderer.WaitIdle();
        mImageIndex = std::nullopt;
    }
}

std::optional<uint32_t> CWindow::GetImageIndex() const {
    return mImageIndex;
}

SDL_Window* CWindow::GetSDLWindow() const {
    return mSDLWindow;
}

} // namespace Renderer
