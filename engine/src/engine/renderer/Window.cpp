#include "engine/renderer/Window.h"

#include <functional>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include "engine/component/CameraComponent.h"
#include "engine/component/ComponentManager.h"
#include "engine/component/MeshComponent.h"
#include "engine/component/MovementComponent.h"
#include "engine/component/SpriteComponent.h"
#include "engine/component/TextUIComponent.h"
#include "engine/component/TransformComponent.h"
#include "engine/core/DataTypes.h"
#include "engine/core/GameObject.h"
#include "engine/core/Mesh.h"
#include "engine/debug/Overlord.h"
#include "engine/font/Police.h"
#include "engine/material/AbstractMaterial.h"
#include "engine/material/MaterialManager.h"
#include "engine/renderer/MeshRenderer.h"
#include "engine/renderer/Renderables.h"
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

void ExtractMeshes(
    Core::CGameObject& root, Component::CComponentManager& componentManager,
    Renderer::CRenderables<Renderer::SMeshRenderable>& meshRenderables,
    Renderer::CRenderables<Renderer::STextRenderable>& textRenderables,
    glm::mat4 transform = glm::mat4(1.0f)) {
    if (!root.IsVisible()) {
        return;
    }
    auto gameObjectId = root.GetId();
    auto& transformComponent =
        *componentManager.GetComponent<Component::CTransformComponent>(
            root.GetId());
    bool alreadyUpdated = false;
    if (root.GetDirtyFlags() != Core::EDirtyType::None) {
        if (auto* meshComponent =
                componentManager.GetComponent<Component::CMeshComponent>(
                    root.GetId())) {
            Renderer::SMeshRenderable renderable{};
            renderable.id = gameObjectId;
            renderable.transform = transform;
            renderable.materialId =
                meshComponent->GetMesh()->GetMaterial()->GetId();
            renderable.meshHash = meshComponent->GetMesh()->GetHash();
            renderable.color = meshComponent->GetColor();
            renderable.textureIndex = meshComponent->GetTextureIndex();

            meshRenderables.AddRenderable(
                renderable, Core::RequireReordering(root.GetDirtyFlags()));
            transformComponent.UpdateMatrix(transform,
                                            meshComponent->GetSize());
            alreadyUpdated = true;
        }
        if (auto* textComponent =
                componentManager.GetComponent<Component::CTextUIComponent>(
                    root.GetId())) {

            Renderer::STextRenderable renderable{};
            renderable.id = gameObjectId;
            renderable.instancesData = textComponent->GetInstances();

            transformComponent.UpdateMatrix(transform,
                                            textComponent->GetSize());
            for (auto& instance : renderable.instancesData) {
                instance.modelMatrix = transform * instance.modelMatrix;
            }
            textRenderables.AddRenderable(
                renderable, Core::RequireReordering(root.GetDirtyFlags()));
            alreadyUpdated = true;
        }
    }
    if (!alreadyUpdated) {
        transformComponent.UpdateMatrix(transform);
    }
    root.ClearDirtyFlags();

    for (auto& child : root.GetChildren()) {
        ExtractMeshes(*child, componentManager, meshRenderables,
                      textRenderables, transform);
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
    Renderer::CRenderables<Renderer::SMeshRenderable> meshRenderables;
    Renderer::CRenderables<Renderer::STextRenderable> textRenderables;

    for (auto& child : scene.GetRoot().GetChildren()) {
        ExtractMeshes(*child, componentManager, meshRenderables,
                      textRenderables);
    }

    mRendererManager.GenerateInstances(meshRenderables, textRenderables);
    VkCommandBuffer commandBuffer =
        mVulkanContext.GetCommandBuffer(mImageIndex.value());
    mRendererManager.Render(commandBuffer,
                            mDescriptorStorage.GetDescriptorSet(),
                            scene.GetActiveCamera(), scene.GetUICamera());
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