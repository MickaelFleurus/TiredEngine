#include "engine/renderer/RendererManager.h"

#include "engine/component/ComponentManager.h"
#include "engine/core/Camera.h"
#include "engine/material/MaterialManager.h"
#include "engine/renderer/Renderables.h"
#include "engine/vulkan/BufferHandler.h"
#include "engine/vulkan/Constants.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace Renderer {

CRendererManager::CRendererManager(Vulkan::CBufferHandler& bufferHandler,
                                   Material::CMaterialManager& materialManager,
                                   CTransformManager& transformManager)
    : mVertexBuffer(
          bufferHandler.Get<Core::SVertex>(Vulkan::kVerticesBufferIndex))
    , mIndexesBuffer(
          bufferHandler.Get<Core::IndexType>(Vulkan::kIndicesBufferIndex))
    , mInstanceBuffer(
          bufferHandler.Get<Core::SInstanceData>(Vulkan::kInstanceBufferIndex))
    , mIndirectDrawBuffer(bufferHandler.Get<Core::SIndirectDrawCommand>(
          Vulkan::kInstanceInfoBufferIndex))
    , mTextInstanceBuffer(bufferHandler.Get<Core::SUIInstanceData>(
          Vulkan::kTextInstanceBufferIndex))
    , mUIVertexBuffer(
          bufferHandler.Get<Core::SUIVertex>(Vulkan::kUIVerticesBufferIndex))
    , mMaterialManager(materialManager)
    , mTransformManager(transformManager)
    , mMeshRenderer(mVertexBuffer, mIndexesBuffer, mInstanceBuffer,
                    mIndirectDrawBuffer, mTransformManager)
    , mTextRenderer(mUIVertexBuffer, mIndexesBuffer, mTextInstanceBuffer,
                    mIndirectDrawBuffer, mTransformManager) {
}

CRendererManager::~CRendererManager() = default;

void CRendererManager::Prepare() {
    mTextRenderer.Prepare();
}

CMeshRenderer& CRendererManager::GetMeshRenderer() {
    return mMeshRenderer;
}

CTextRenderer& CRendererManager::GetTextRenderer() {
    return mTextRenderer;
}

void CRendererManager::FreeSceneData() {
    mTextRenderer.Free();
    mMeshRenderer.Free();
}

void CRendererManager::GenerateInstances(
    Component::CComponentManager& componentManager) {
    mMeshRenderer.UpdateInstances(
        componentManager.GetComponents(Component::EComponentType::Mesh));
    mTextRenderer.UpdateInstances(
        componentManager.GetComponents(Component::EComponentType::TextUI));
}

void CRendererManager::Render(VkCommandBuffer commandBuffer,
                              VkDescriptorSet descriptorSet,
                              Core::CCamera& camera, Core::CCamera& uiCamera) {
    if (descriptorSet == VK_NULL_HANDLE) {
        LOG_ERROR("Invalid descriptor set passed to Render!");
        return;
    }
    mVertexBuffer.Upload();
    mIndexesBuffer.Upload();
    mInstanceBuffer.Upload();
    mIndirectDrawBuffer.Upload();
    mTextInstanceBuffer.Upload();
    mUIVertexBuffer.Upload();

    VkDeviceSize offsets[] = {0, 0};
    VkBuffer vertexBuffers[] = {mVertexBuffer.GetBuffer(),
                                mInstanceBuffer.GetBuffer()};
    vkCmdBindVertexBuffers(commandBuffer, 0, 2, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, mIndexesBuffer.GetBuffer(), 0,
                         VK_INDEX_TYPE_UINT32);
    Material::CAbstractMaterial* currentMaterial = nullptr;
    for (auto& [materialId, instanceRanges] :
         mMeshRenderer.GetIndirectDrawRanges()) {
        currentMaterial = mMaterialManager.GetMaterialById(materialId);
        if (!currentMaterial) {
            currentMaterial = mMaterialManager.GetorCreateMaterial(
                Material::EMaterialType::Normal);
        }
        auto pipeline = currentMaterial->GetPipeline();
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline);

        VkPipelineLayout pipelineLayout = currentMaterial->GetPipelineLayout();
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipelineLayout, 0, 1, &descriptorSet, 0,
                                nullptr);

        Core::SPushConstantData pushConstantData{};
        pushConstantData.viewMatrix = camera.GetViewMatrix();
        pushConstantData.projectionMatrix = camera.GetProjectionMatrix();
        vkCmdPushConstants(
            commandBuffer, pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
            sizeof(Core::SPushConstantData), &pushConstantData);

        for (const auto& instanceRange : instanceRanges) {
            for (std::size_t i = 0; i < instanceRange.count; ++i) {
                vkCmdDrawIndexedIndirect(commandBuffer,
                                         mIndirectDrawBuffer.GetBuffer(),
                                         (instanceRange.first + i) *
                                             sizeof(Core::SIndirectDrawCommand),
                                         1, sizeof(Core::SIndirectDrawCommand));
            }
        }
    }

    const auto& textIndirectDrawRanges = mTextRenderer.GetIndirectDrawRange();

    if (!textIndirectDrawRanges.empty()) {
        VkBuffer textVertexBuffers[] = {mUIVertexBuffer.GetBuffer(),
                                        mTextInstanceBuffer.GetBuffer()};
        vkCmdBindVertexBuffers(commandBuffer, 0, 2, textVertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, mIndexesBuffer.GetBuffer(), 0,
                             VK_INDEX_TYPE_UINT32);

        currentMaterial =
            mMaterialManager.GetorCreateMaterial(Material::EMaterialType::UI);
        if (!currentMaterial) {
            LOG_ERROR("Failed to get/create Text material!");
            return;
        }
        auto pipeline = currentMaterial->GetPipeline();
        if (!pipeline) {
            LOG_ERROR("Text material has no pipeline!");
            return;
        }

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline);
        VkPipelineLayout pipelineLayout = currentMaterial->GetPipelineLayout();
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipelineLayout, 0, 1, &descriptorSet, 0,
                                nullptr);

        Core::SPushConstantData pushConstantData{};
        pushConstantData.viewMatrix = uiCamera.GetViewMatrix();
        pushConstantData.projectionMatrix = uiCamera.GetProjectionMatrix();

        vkCmdPushConstants(
            commandBuffer, pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
            sizeof(Core::SPushConstantData), &pushConstantData);

        for (const auto& instanceRange : textIndirectDrawRanges) {
            for (std::size_t i = 0; i < instanceRange.count; ++i) {
                uint64_t indirectOffset = (instanceRange.first + i) *
                                          sizeof(Core::SIndirectDrawCommand);
                vkCmdDrawIndexedIndirect(
                    commandBuffer, mIndirectDrawBuffer.GetBuffer(),
                    indirectOffset, 1, sizeof(Core::SIndirectDrawCommand));
            }
        }
    }
}

} // namespace Renderer
