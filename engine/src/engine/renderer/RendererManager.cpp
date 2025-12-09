#include "engine/renderer/RendererManager.h"

#include "engine/core/Camera.h"
#include "engine/material/MaterialManager.h"
#include "engine/vulkan/BufferHandler.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace Renderer {
CRendererManager::CRendererManager(Vulkan::CBufferHandler& bufferHandler,
                                   Material::CMaterialManager& materialManager)
    : mVertexBuffer(bufferHandler.Get<Core::SVertex>())
    , mIndexesBuffer(bufferHandler.Get<Core::IndexType>())
    , mInstanceBuffer(bufferHandler.Get<Core::SInstanceData>())
    , mIndirectDrawBuffer(bufferHandler.Get<Core::SIndirectDrawCommand>())
    , mTextInstanceBuffer(bufferHandler.Get<Core::STextInstanceData>())
    , mMaterialManager(materialManager)
    , mMeshRenderer(mVertexBuffer, mIndexesBuffer, mInstanceBuffer,
                    mIndirectDrawBuffer)
    , mTextRenderer(mVertexBuffer, mIndexesBuffer, mTextInstanceBuffer,
                    mIndirectDrawBuffer) {
}

CRendererManager::~CRendererManager() = default;

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
    const std::vector<SRenderable>& renderables) {
    mMeshRenderer.UpdateInstances(renderables);
}

void CRendererManager::Render(VkCommandBuffer commandBuffer,
                              VkDescriptorSet descriptorSet,
                              Core::CCamera& camera) {

    mVertexBuffer.Upload();
    mIndexesBuffer.Upload();
    mInstanceBuffer.Upload();
    mIndirectDrawBuffer.Upload();
    mTextInstanceBuffer.Upload();

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
            commandBuffer, currentMaterial->GetPipelineLayout(),
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
            sizeof(Core::SPushConstantData), &pushConstantData);
        // mMeshRenderer.Draw(commandBuffer);

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
}

} // namespace Renderer