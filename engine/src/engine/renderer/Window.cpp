#include "engine/renderer/Window.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include "engine/component/CameraComponent.h"
#include "engine/component/ComponentManager.h"
#include "engine/component/MovementComponent.h"
#include "engine/component/SpriteComponent.h"
#include "engine/component/TextComponent.h"
#include "engine/component/TransformComponent.h"
#include "engine/core/Camera.h"
#include "engine/core/GameObject.h"
#include "engine/debug/Overlord.h"
#include "engine/font/Police.h"
#include "engine/material/AbstractMaterial.h"
#include "engine/renderer/RendererUtils.h"
#include "engine/renderer/SamplerCache.h"
#include "engine/renderer/TextRenderer.h"
#include "engine/renderer/VulkanRenderer.h"
#include "engine/scene/AbstractScene.h"
#include "engine/system/System.h"
#include "engine/utils/Logger.h"

#include <functional>
#include <vector>

namespace {

constexpr auto kSDLWindowDeleter = [](SDL_Window* window) {
    if (window) {
        SDL_DestroyWindow(window);
    }
};

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

class CWindow::CImpl {
public:
    CImpl(const System::CSystem& system, SDL_Window* window)
        : mSystem(system)
        , mSDLWindow(std::unique_ptr<SDL_Window, decltype(kSDLWindowDeleter)>(
              window, kSDLWindowDeleter))
        , mViewport(
              GetViewport(mSDLWindow.get(), mSystem.GetDisplayParameters()))
        , mScissor(GetScissor(mViewport))

    {
        SDL_InitSubSystem(SDL_INIT_VIDEO);
        mRenderer.Init(mSDLWindow.get(), mSystem);

        mTextRenderer = std::make_unique<CTextRenderer>(
            mRenderer.GetVulkanDevice().device,
            mRenderer.GetVulkanPhysicalDevice().memoryProperties);
    }

    ~CImpl() {
        EndRender();
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    void ExtractRenderable(Core::CGameObject& root,
                           Component::CComponentManager& componentManager,
                           std::vector<SRenderable>& renderables,
                           glm::mat4 transform = glm::mat4(1.0f)) {
        if (!root.IsVisible()) {
            return;
        }
        auto& transformComponent =
            *componentManager.getComponent<Component::CTransformComponent>(
                root.getId());

        if (auto* spriteComponent =
                componentManager.getComponent<Component::CSpriteComponent>(
                    root.getId())) {
            transformComponent.UpdateMatrix(transform,
                                            spriteComponent->getSize());
            SRenderable renderable{};
            renderable.transform = transform;
            renderable.layer = ERenderLayer::World;
            renderable.depth = transform[3][2]; // Z position
            renderable.GenerateSortKey();

            renderables.push_back(renderable);
        } else if (auto* textComponent =
                       componentManager.getComponent<Component::CTextComponent>(
                           root.getId())) {

            transformComponent.UpdateMatrix(transform,
                                            textComponent->getSize());
            SRenderable renderable{};
            renderable.material = &textComponent->getPolice()->GetMaterial();
            renderable.transform = transform;
            renderable.layer = ERenderLayer::UI;
            renderable.depth = transform[3][2];

            renderable.vertexBuffer = mTextRenderer->GetQuadVertexBuffer();
            renderable.indexBuffer = mTextRenderer->GetQuadIndexBuffer();
            renderable.instanceBuffer = textComponent->GetInstanceBuffer();

            renderable.instanceCount = textComponent->GetInstanceCount();
            renderable.indexCount = 6;

            renderable.GenerateSortKey();
            renderables.push_back(renderable);
        } else {
            transformComponent.UpdateMatrix(transform);
        }

        for (auto& child : root.getChildren()) {
            ExtractRenderable(*child, componentManager, renderables, transform);
        }
    }

    void Render(Scene::CAbstractScene& scene,
                Component::CComponentManager& componentManager) {
        auto& camera = scene.GetActiveCamera();

        std::vector<SRenderable> renderables;
        for (auto& child : scene.GetRoot().getChildren()) {
            ExtractRenderable(*child, componentManager, renderables);
        }

        std::sort(renderables.begin(), renderables.end());

        std::vector<SRenderable> worldRenderables;
        std::vector<SRenderable> uiRenderables;

        for (const auto& r : renderables) {
            if (r.layer == ERenderLayer::UI || r.layer == ERenderLayer::Debug) {
                uiRenderables.push_back(r);
            } else {
                worldRenderables.push_back(r);
            }
        }

        // === WORLD PASS ===
        RenderPass(worldRenderables, scene.GetActiveCamera());

        // === UI PASS ===
        RenderPass(uiRenderables, scene.GetUICamera());
    }

    bool BeginRender() {
        auto& queue = mRenderer.GetQueue();
        queue.WaitIdle();
        mImageIndex = queue.AcquireNextImage();
        mRenderer.BeginRenderPass(mImageIndex.value(), mViewport, mScissor);
        return mImageIndex.has_value();
    }

    void EndRender() {
        if (mImageIndex.has_value()) {
            mRenderer.EndRenderPass(mImageIndex.value());
            auto& queue = mRenderer.GetQueue();
            queue.SubmitAsync(mRenderer.GetCommandBuffer(mImageIndex.value()));
            queue.Present(mImageIndex.value());
            mImageIndex = std::nullopt;
        }
    }

    CTextRenderer& GetTextRenderer() {
        return *mTextRenderer;
    }
    const VulkanRenderer& GetVulkanRenderer() const {
        return mRenderer;
    }

    VkCommandBuffer GetCommandBuffer() {
        if (!mImageIndex.has_value()) {
            return VK_NULL_HANDLE;
        }
        return mRenderer.GetCommandBuffer(mImageIndex.value());
    }

    SDL_Window* GetSDLWindow() const {
        return mSDLWindow.get();
    }

private:
    void RenderPass(const std::vector<SRenderable>& renderables,
                    Core::CCamera& camera) {
        SDL_GPUGraphicsPipeline* currentPipeline = nullptr;
        SDL_GPUTexture* currentTexture = nullptr;

        // auto pushConstants = camera.GetMatrices();

        // for (const auto& renderable : renderables) {
        //     if (!renderable.material) {
        //         continue;
        //     }

        //     // Bind pipeline if changed
        //     SDL_GPUGraphicsPipeline* pipeline =
        //         renderable.material->GetPipeline();
        //     if (pipeline && pipeline != currentPipeline) {
        //         SDL_BindGPUGraphicsPipeline(mSDLRenderPass, pipeline);
        //         currentPipeline = pipeline;
        //     }

        //     // Bind texture if changed
        //     SDL_GPUTexture* texture = renderable.material->GetTexture();
        //     if (texture && texture != currentTexture) {
        //         SDL_GPUTextureSamplerBinding binding = {};
        //         binding.texture = texture;
        //         binding.sampler = mSamplerCache.GetLinear();
        //         SDL_BindGPUFragmentSamplers(mSDLRenderPass, 0, &binding, 1);
        //         currentTexture = texture;
        //     }
        //     // Bind vertex buffers
        //     if (renderable.vertexBuffer) {
        //         SDL_GPUBufferBinding vertexBinding = {};
        //         vertexBinding.buffer = renderable.vertexBuffer;
        //         vertexBinding.offset = 0;
        //         SDL_BindGPUVertexBuffers(mSDLRenderPass, 0, &vertexBinding,
        //         1);
        //     }

        //     // Bind instance buffer if present
        //     if (renderable.instanceBuffer) {
        //         SDL_GPUBufferBinding instanceBinding = {};
        //         instanceBinding.buffer = renderable.instanceBuffer;
        //         instanceBinding.offset = 0;
        //         SDL_BindGPUVertexBuffers(mSDLRenderPass, 1, &instanceBinding,
        //                                  1);
        //     }

        //     // Bind index buffer
        //     if (renderable.indexBuffer) {
        //         SDL_GPUBufferBinding indexBinding = {};
        //         indexBinding.buffer = renderable.indexBuffer;
        //         indexBinding.offset = 0;
        //         SDL_BindGPUIndexBuffer(mSDLRenderPass, &indexBinding,
        //                                SDL_GPU_INDEXELEMENTSIZE_16BIT);
        //     }

        //     // Set model matrix for this renderable
        //     pushConstants.model = renderable.transform;

        //     SDL_PushGPUVertexUniformData(mSDLCommandBuffer, 0,
        //     &pushConstants,
        //                                  sizeof(Core::CCamera::SMatrices));

        //     // Draw
        //     if (renderable.instanceCount > 0 && renderable.indexBuffer) {
        //         SDL_DrawGPUIndexedPrimitives(mSDLRenderPass,
        //                                      renderable.indexCount,
        //                                      renderable.instanceCount, 0, 0,
        //                                      0);
        //     } else if (renderable.indexCount > 0 && renderable.indexBuffer) {
        //         SDL_DrawGPUIndexedPrimitives(mSDLRenderPass,
        //                                      renderable.indexCount, 1, 0, 0,
        //                                      0);
        //     }
        // }
    }

    const System::CSystem& mSystem;
    std::unique_ptr<SDL_Window, decltype(kSDLWindowDeleter)> mSDLWindow;
    VulkanRenderer mRenderer;

    std::optional<uint32_t> mImageIndex = std::nullopt;

    VkViewport mViewport;
    VkRect2D mScissor;

    CSamplerCache* mSamplerCache;
    std::unique_ptr<CTextRenderer> mTextRenderer;
};

CWindow::CWindow(System::CSystem& system) : mSystem(system) {
}

CWindow::~CWindow() = default;

bool CWindow::Initialize() {
    const std::string& gameName = mSystem.GetGameName();
    const System::SDisplayParameter& displayParams =
        mSystem.GetDisplayParameters();

    SDL_WindowFlags windowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN;
    if (displayParams.fullscreen) {
        windowFlags |= SDL_WINDOW_FULLSCREEN;
    }
    auto* window = SDL_CreateWindow(gameName.c_str(), displayParams.width,
                                    displayParams.height, windowFlags);
    if (!window) {
        return false;
    }
    mImpl = std::make_unique<CImpl>(mSystem, window);
    return true;
}

bool CWindow::BeginRender() {
    return mImpl->BeginRender();
}

void CWindow::Render(Scene::CAbstractScene& scene,
                     Component::CComponentManager& componentManager) {
    mImpl->Render(scene, componentManager);
}

void CWindow::EndRender() {
    mImpl->EndRender();
}

SDL_Window* CWindow::GetSDLWindow() const {
    return mImpl->GetSDLWindow();
}

CTextRenderer& CWindow::GetTextRenderer() {
    return mImpl->GetTextRenderer();
}

const VulkanRenderer& CWindow::GetVulkanRenderer() const {
    return mImpl->GetVulkanRenderer();
}

VkCommandBuffer CWindow::GetCommandBuffer() {
    return mImpl->GetCommandBuffer();
}

} // namespace Renderer