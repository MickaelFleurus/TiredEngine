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
#include "engine/renderer/DescriptorLayoutStorage.h"
#include "engine/renderer/RendererUtils.h"
#include "engine/renderer/SamplerCache.h"
#include "engine/renderer/TextRenderer.h"
#include "engine/scene/AbstractScene.h"
#include "engine/system/System.h"
#include "engine/utils/Logger.h"
#include "engine/vulkan/VulkanContext.h"
#include "engine/vulkan/VulkanInitializer.h"
#include "engine/vulkan/VulkanRendering.h"

#include <functional>
#include <vector>

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

CWindow::CWindow(System::CSystem& system, SDL_Window* window,
                 Vulkan::IVulkanContextGetter& vulkanContext,
                 Vulkan::CVulkanRendering& renderer)
    : mSystem(system)
    , mVulkanContext(vulkanContext)
    , mRenderer(renderer)
    , mSDLWindow(window)
    , mViewport(GetViewport(window, mSystem.GetDisplayParameters()))
    , mScissor(GetScissor(mViewport))

{
}

CWindow::~CWindow() {
    EndRender();
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void CWindow::ExtractRenderable(Core::CGameObject& root,
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
        transformComponent.UpdateMatrix(transform, spriteComponent->getSize());
        SRenderable renderable{};
        renderable.transform = transform;
        renderable.layer = ERenderLayer::World;
        renderable.depth = transform[3][2]; // Z position
        renderable.GenerateSortKey();

        renderables.push_back(renderable);
    } else if (auto* textComponent =
                   componentManager.getComponent<Component::CTextComponent>(
                       root.getId())) {

        transformComponent.UpdateMatrix(transform, textComponent->getSize());
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

void CWindow::Render(Scene::CAbstractScene& scene,
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

bool CWindow::BeginRender() {
    mRenderer.WaitIdle();
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
            mVulkanContext.GetCommandBuffer(mImageIndex.value()));
        mRenderer.Present(mImageIndex.value());
        mImageIndex = std::nullopt;
    }
}

void CWindow::RenderPass(const std::vector<SRenderable>& renderables,
                         Core::CCamera& camera) {
    VkCommandBuffer commandBuffer = GetCommandBuffer();
    if (commandBuffer == VK_NULL_HANDLE) {
        return;
    }
    for (const auto& renderable : renderables) {
        vkCmdPushConstants(commandBuffer,
                           renderable.material->GetPipelineLayout(),
                           VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4),
                           &renderable.transform);
    }
}

} // namespace Renderer