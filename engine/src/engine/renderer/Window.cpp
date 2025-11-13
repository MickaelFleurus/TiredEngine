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
#include "engine/scene/AbstractScene.h"
#include "engine/system/System.h"
#include "engine/utils/Logger.h"
#include "engine/utils/VulkanUtils.h"

#include <functional>

namespace {

constexpr auto kSDLWindowDeleter = [](SDL_Window* window) {
    if (window) {
        SDL_DestroyWindow(window);
    }
};

SDL_GPUViewport GetViewport(SDL_Window* window,
                            const System::SDisplayParameter& displayParams) {

    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    SDL_GPUViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.w = static_cast<float>(std::min(displayParams.width, w));
    viewport.h = static_cast<float>(std::min(displayParams.height, h));
    viewport.min_depth = 0.0f;
    viewport.max_depth = 1.0f;
    return viewport;
}

SDL_Rect GetScissor(SDL_GPUViewport viewport) {
    SDL_Rect scissor;
    scissor.x = 0;
    scissor.y = 0;
    scissor.w = viewport.w;
    scissor.h = viewport.h;
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
        , mTextRenderer(nullptr)
        , mSamplerCache(nullptr)

    {
        SDL_InitSubSystem(SDL_INIT_VIDEO);

        uint32_t extensionCount = 0;
        if (!SDL_Vulkan_GetInstanceExtensions(&extensionCount)) {
            LOG_FATAL("Failed to get Vulkan extension count: {}",
                      SDL_GetError());
            return;
        }

        auto extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
        if (!extensions) {
            LOG_FATAL("Failed to get Vulkan extensions: {}", SDL_GetError());
            return;
        }
        std::vector<const char*> extensionsVec(extensions,
                                               extensions + extensionCount);

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = mSystem.GetGameName().c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "TiredEngine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

#ifdef DEBUG_BUILD
        // Check if validation layers are available
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        bool validationLayerFound = false;
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerProperties.layerName,
                       "VK_LAYER_KHRONOS_validation") == 0) {
                validationLayerFound = true;
                break;
            }
        }

        if (validationLayerFound) {
            const char* validationLayers[] = {"VK_LAYER_KHRONOS_validation"};
            createInfo.enabledLayerCount = 1;
            createInfo.ppEnabledLayerNames = validationLayers;
            LOG_INFO("Vulkan validation layers enabled");

            // Check if debug utils extension is available
            uint32_t extensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                                   nullptr);
            std::vector<VkExtensionProperties> availableExtensions(
                extensionCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                                   availableExtensions.data());

            bool hasDebugUtils = false;
            for (const auto& ext : availableExtensions) {
                if (strcmp(ext.extensionName,
                           VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0) {
                    hasDebugUtils = true;
                    break;
                }
            }

            if (hasDebugUtils) {
                extensionsVec.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
                LOG_INFO("Vulkan debug utils extension enabled");
            } else {
                LOG_WARNING(
                    "Debug utils extension not available on this platform");
            }
        } else {
            createInfo.enabledLayerCount = 0;
            LOG_WARNING("VK_LAYER_KHRONOS_validation not found - validation "
                        "disabled. Install Vulkan SDK to enable validation.");
        }
#else
        createInfo.enabledLayerCount = 0;
#endif
#ifdef __APPLE__
        extensionsVec.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

        createInfo.enabledExtensionCount =
            static_cast<uint32_t>(extensionsVec.size());
        createInfo.ppEnabledExtensionNames = extensionsVec.data();

        if (auto result = vkCreateInstance(&createInfo, nullptr, &mInstance);
            result != VK_SUCCESS) {
            LOG_FATAL("Couldn't create Vulkan instance! Error code: {}",
                      static_cast<int>(result));
            return;
        }

        mSurface = Utils::CreateVulkanSurface(mSDLWindow.get(), mInstance);
        mPhysicalDevice = Utils::PickPhysicalDevice(mInstance, mSurface);
        mVulkanDevice = Utils::CreateLogicalDevice(mPhysicalDevice, mSurface);
        mSwapChainSupport =
            Utils::QuerySwapChainSupport(mPhysicalDevice, mSurface);
        mSwapchain = Utils::CreateSwapchain(
            mPhysicalDevice, mVulkanDevice.device, mSurface, mSDLWindow.get(),
            mVulkanDevice.graphicsFamily, mVulkanDevice.presentFamily);
        mRenderPass = Utils::CreateRenderPass(mVulkanDevice.device,
                                              mSwapchain.imageFormat);
        Utils::CreateVulkanCallback(mInstance);
    }

    ~CImpl() {
        EndRender();
        vkDestroyInstance(mInstance, nullptr);
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

            renderable.vertexBuffer = mTextRenderer.GetQuadVertexBuffer();
            renderable.indexBuffer = mTextRenderer.GetQuadIndexBuffer();
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

        // SDL_SetGPUViewport(mSDLRenderPass, &mViewport);
        // SDL_SetGPUScissor(mSDLRenderPass, &mScissor);

        // === WORLD PASS ===
        RenderPass(worldRenderables, scene.GetActiveCamera());

        // === UI PASS ===
        RenderPass(uiRenderables, scene.GetUICamera());
    }

    SDL_Window* GetWindow() const {
        return mSDLWindow.get();
    }

    bool PrepareRender() {
        // mSDLCommandBuffer = SDL_AcquireGPUCommandBuffer(mSDLDevice.get());
        // if (!mSDLCommandBuffer) {
        //     LOG_ERROR("Failed to acquire command buffer: {}",
        //     SDL_GetError());

        //     return false;
        // }
        return false;
    }

    bool BeginRender() {
        // SDL_GPUTexture* swapchainTexture;
        // if (!SDL_WaitAndAcquireGPUSwapchainTexture(
        //         mSDLCommandBuffer, mSDLWindow.get(), &swapchainTexture,
        //         nullptr, nullptr)) {
        //     LOG_ERROR("Failed to acquire swapchain texture: {}",
        //               SDL_GetError());
        //     return false;
        // }

        // if (!swapchainTexture) {
        //     LOG_ERROR("Swapchain texture is null");
        //     return false;
        // }

        // SDL_GPUColorTargetInfo colorTarget = {};
        // colorTarget.texture = swapchainTexture;
        // colorTarget.clear_color = {0.0f, 0.0f, 0.0f, 1.0f}; // Black
        // background colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
        // colorTarget.store_op = SDL_GPU_STOREOP_STORE;

        // mSDLRenderPass =
        //     SDL_BeginGPURenderPass(mSDLCommandBuffer, &colorTarget, 1,
        //     nullptr);
        // if (!mSDLRenderPass) {
        //     LOG_ERROR("Failed to begin render pass: {}", SDL_GetError());
        //     return false;
        // }

        return false;
    }

    void EndRender() {
        // if (mSDLRenderPass) {
        //     SDL_EndGPURenderPass(mSDLRenderPass);
        //     mSDLRenderPass = nullptr;
        // }
        // if (mSDLCommandBuffer) {
        //     SDL_SubmitGPUCommandBuffer(mSDLCommandBuffer);
        //     mSDLCommandBuffer = nullptr;
        // }
    }

    CTextRenderer& GetTextRenderer() {
        return mTextRenderer;
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
    VkInstance mInstance;
    VkSurfaceKHR mSurface;
    VkPhysicalDevice mPhysicalDevice;
    Utils::VulkanDevice mVulkanDevice;
    Utils::SwapChainSupportDetails mSwapChainSupport;
    Utils::VulkanSwapchain mSwapchain;
    VkRenderPass mRenderPass;

    SDL_Rect mScissor;

    CSamplerCache mSamplerCache;
    CTextRenderer mTextRenderer;
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

bool CWindow::PrepareRender() {
    return mImpl->PrepareRender();
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

SDL_Window* CWindow::Get() const {
    return mImpl->GetWindow();
}

CTextRenderer& CWindow::GetTextRenderer() {
    return mImpl->GetTextRenderer();
}

} // namespace Renderer