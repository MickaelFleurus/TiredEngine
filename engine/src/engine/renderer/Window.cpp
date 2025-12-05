#include "engine/renderer/Window.h"

#include "engine/component/CameraComponent.h"
#include "engine/component/ComponentManager.h"
#include "engine/component/MeshComponent.h"
#include "engine/component/MovementComponent.h"
#include "engine/component/SpriteComponent.h"
#include "engine/component/TextComponent.h"
#include "engine/component/TransformComponent.h"
#include "engine/core/Camera.h"
#include "engine/core/DataTypes.h"
#include "engine/core/GameObject.h"
#include "engine/core/Mesh.h"
#include "engine/debug/Overlord.h"
#include "engine/font/Police.h"
#include "engine/material/AbstractMaterial.h"
#include "engine/material/MaterialManager.h"
#include "engine/renderer/MeshRenderer.h"
#include "engine/renderer/RendererManager.h"
#include "engine/renderer/RendererUtils.h"
#include "engine/renderer/TextRenderer.h"
#include "engine/scene/AbstractScene.h"
#include "engine/system/System.h"
#include "engine/utils/Logger.h"
#include "engine/vulkan/BufferHandleWrapper.h"
#include "engine/vulkan/BufferHandler.h"
#include "engine/vulkan/DescriptorStorage.h"
#include "engine/vulkan/VulkanContext.h"
#include "engine/vulkan/VulkanRendering.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_vulkan.h>
#include <functional>
#include <vector>
#include <vulkan/vulkan.h>

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

void ExtractRenderable(Core::CGameObject& root,
                       Component::CComponentManager& componentManager,
                       std::vector<Renderer::SRenderable>& renderables,
                       glm::mat4 transform = glm::mat4(1.0f)) {
    if (!root.IsVisible()) {
        return;
    }
    auto& transformComponent =
        *componentManager.getComponent<Component::CTransformComponent>(
            root.getId());

    if (auto* meshComponent =
            componentManager.getComponent<Component::CMeshComponent>(
                root.getId())) {
        transformComponent.UpdateMatrix(transform);
        Renderer::SRenderable renderable{};
        renderable.transform = transform;
        renderable.material = meshComponent->GetMesh()->GetMaterial();
        renderable.meshHash = meshComponent->GetMesh()->GetHash();
        renderable.color = meshComponent->GetColor();
        renderable.textureIndex = meshComponent->GetTextureIndex();

        renderables.push_back(renderable);
    } else {
        transformComponent.UpdateMatrix(transform);
    }

    for (auto& child : root.getChildren()) {
        ExtractRenderable(*child, componentManager, renderables, transform);
    }
}

} // namespace

namespace Renderer {

CWindow::CWindow(System::CSystem& system, SDL_Window* window,
                 Vulkan::CVulkanContext& vulkanContext,
                 Vulkan::CVulkanRendering& renderer,
                 Vulkan::CBufferHandler& bufferHandler,
                 Material::CMaterialManager& materialManager,
                 Vulkan::CDescriptorStorage& descriptorStorage,
                 CRendererManager& rendererManager)
    : mSystem(system)
    , mVulkanContext(vulkanContext)
    , mRenderer(renderer)
    , mBufferHandler(bufferHandler)
    , mMaterialManager(materialManager)
    , mDescriptorStorage(descriptorStorage)
    , mRendererManager(rendererManager)
    , mSDLWindow(window)
    , mViewport(GetViewport(window, mSystem.GetDisplayParameters()))
    , mScissor(GetScissor(mViewport))

{
}

CWindow::~CWindow() {
    EndRender();
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void CWindow::Render(Scene::CAbstractScene& scene,
                     Component::CComponentManager& componentManager) {
    auto& camera = scene.GetUICamera();

    std::vector<SRenderable> renderables;
    for (auto& child : scene.GetRoot().getChildren()) {
        ExtractRenderable(*child, componentManager, renderables);
    }

    std::sort(renderables.begin(), renderables.end());
    if (renderables != mRenderables) {
        mRendererManager.RebuildInstances(renderables);
        mRenderables = std::move(renderables);
    }

    VkCommandBuffer commandBuffer =
        mVulkanContext.GetCommandBuffer(mImageIndex.value());
    mRendererManager.Render(commandBuffer,
                            mDescriptorStorage.GetDescriptorSet(),
                            scene.GetActiveCamera());
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

        mRenderer.SubmitAsync(
            mVulkanContext.GetCommandBuffer(mImageIndex.value()),
            mImageIndex.value());
        mRenderer.Present(mImageIndex.value());
        mRenderer.WaitIdle();
        mImageIndex = std::nullopt;
    }
}

} // namespace Renderer