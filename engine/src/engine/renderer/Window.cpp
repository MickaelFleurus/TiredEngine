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
#include "engine/material/MaterialFactory.h"
#include "engine/renderer/RendererUtils.h"

#include "engine/renderer/TextRenderer.h"
#include "engine/scene/AbstractScene.h"
#include "engine/system/System.h"
#include "engine/utils/Logger.h"
#include "engine/vulkan/BufferHandler.h"
#include "engine/vulkan/DescriptorStorage.h"
#include "engine/vulkan/VulkanContext.h"
#include "engine/vulkan/VulkanRendering.h"

#include <functional>
#include <vector>

namespace {

struct SPushConstantData {
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
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

    if (auto* spriteComponent =
            componentManager.getComponent<Component::CSpriteComponent>(
                root.getId())) {
        transformComponent.UpdateMatrix(transform, spriteComponent->getSize());
        Renderer::SRenderable renderable{};
        renderable.transform = transform;
        renderable.layer = Renderer::ERenderLayer::World;
        renderable.depth = transform[3][2]; // Z position
        renderable.GenerateSortKey();

        renderables.push_back(renderable);
    } else {
        transformComponent.UpdateMatrix(transform);
    }

    for (auto& child : root.getChildren()) {
        ExtractRenderable(*child, componentManager, renderables, transform);
    }
}

void RenderPass(const std::vector<Renderer::SRenderable>& renderables,
                Core::CCamera& camera, VkCommandBuffer commandBuffer) {

    SPushConstantData pushConstants{};
    pushConstants.viewMatrix = camera.GetViewMatrix();
    pushConstants.projectionMatrix = camera.GetProjectionMatrix();
    Material::AbstractMaterial* currentMaterial = nullptr;
    for (const auto& renderable : renderables) {
        if (!renderable.material) {
            continue;
        }

        if (renderable.material != currentMaterial) {
            currentMaterial = renderable.material;
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                              currentMaterial->GetPipeline());
            vkCmdPushConstants(
                commandBuffer, currentMaterial->GetPipelineLayout(),
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                sizeof(SPushConstantData), &pushConstants);
        }
    }
}

} // namespace

namespace Renderer {

CWindow::CWindow(System::CSystem& system, SDL_Window* window,
                 Vulkan::CVulkanContext& vulkanContext,
                 Vulkan::CVulkanRendering& renderer,
                 Vulkan::CBufferHandler& bufferHandler,
                 CTextRenderer& textRenderer,
                 Material::CMaterialFactory& materialFactory,
                 Vulkan::CDescriptorStorage& descriptorStorage)
    : mSystem(system)
    , mVulkanContext(vulkanContext)
    , mRenderer(renderer)
    , mBufferHandler(bufferHandler)
    , mTextRenderer(textRenderer)
    , mMaterialFactory(materialFactory)
    , mDescriptorStorage(descriptorStorage)
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

    std::vector<SRenderable> worldRenderables;
    std::vector<SRenderable> uiRenderables;

    for (const auto& r : renderables) {
        if (r.layer == ERenderLayer::UI || r.layer == ERenderLayer::Debug) {
            uiRenderables.push_back(r);
        } else {
            worldRenderables.push_back(r);
        }
    }

    VkCommandBuffer commandBuffer =
        mVulkanContext.GetCommandBuffer(mImageIndex.value());
    mBufferHandler.BindBuffers(commandBuffer);
    SPushConstantData pushConstantData{};
    pushConstantData.viewMatrix = camera.GetViewMatrix();
    pushConstantData.projectionMatrix = camera.GetProjectionMatrix();

    auto textMaterial = mMaterialFactory.GetOrCreateTextMaterial();
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      textMaterial->GetPipeline());
    VkPipelineLayout pipelineLayout = textMaterial->GetPipelineLayout();
    VkDescriptorSet descriptorSet = mDescriptorStorage.GetDescriptorSet();
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    vkCmdPushConstants(
        commandBuffer, textMaterial->GetPipelineLayout(),
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
        sizeof(Renderer::SPushConstantData), &pushConstantData);
    mTextRenderer.Update();
    mBufferHandler.BindBuffers(commandBuffer);
    mTextRenderer.DrawTexts(commandBuffer);
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
            mVulkanContext.GetCommandBuffer(mImageIndex.value()));
        mRenderer.Present(mImageIndex.value());
        mRenderer.WaitIdle();
        mImageIndex = std::nullopt;
    }
}

} // namespace Renderer