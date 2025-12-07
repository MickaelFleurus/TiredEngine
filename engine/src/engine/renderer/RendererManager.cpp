#include "engine/renderer/RendererManager.h"

#include "engine/core/Camera.h"
#include "engine/material/MaterialManager.h"
#include "engine/vulkan/BufferHandler.h"

namespace Renderer {
CRendererManager::CRendererManager(Vulkan::CBufferHandler& bufferHandler,
                                   Material::CMaterialManager& materialManager)
    : mBufferHandler(bufferHandler)
    , mMaterialManager(materialManager)
    , mMeshRenderer(mBufferHandler.Get<Core::SVertex>(),
                    mBufferHandler.Get<Core::IndexType>(),
                    mBufferHandler.Get<Core::SInstanceData>(),
                    mBufferHandler.Get<Core::SIndirectDrawCommand>())
    , mTextRenderer(mBufferHandler.Get<Core::SVertex>(),
                    mBufferHandler.Get<Core::IndexType>(),
                    mBufferHandler.Get<Core::STextInstanceData>(),
                    mBufferHandler.Get<Core::SIndirectDrawCommand>()) {
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
    // mBufferHandler.Free();
}

void CRendererManager::UploadSceneData() {
    mTextRenderer.Prepare();
    // mMeshRenderer.Prepare();
    //  mBufferHandler.Upload<Core::SVertex>();
    //  mBufferHandler.Upload<Core::IndexType>();
}

void CRendererManager::GenerateInstances(
    const std::vector<SRenderable>& renderables) {
    mMeshRenderer.UpdateInstances(renderables);
    // mBufferHandler.Upload<Core::SInstanceData>();
    // mBufferHandler.Upload<Core::SIndirectDrawCommand>();
}

void CRendererManager::UpdateInstances(
    const std::vector<SRenderable>& renderables) {
    mMeshRenderer.UpdateInstances(renderables);
    // mTextRenderer.RebuildInstances(renderables);
}

void CRendererManager::Render(VkCommandBuffer commandBuffer,
                              VkDescriptorSet descriptorSet,
                              Core::CCamera& camera) {
    VkDeviceSize offsets[] = {0, 0};
    VkBuffer vertexBuffers[] = {
        mBufferHandler.Get<Core::SVertex>().GetBuffer(),
        mBufferHandler.Get<Core::SInstanceData>().GetBuffer()};

    vkCmdBindVertexBuffers(commandBuffer, 0, 2, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer,
                         mBufferHandler.Get<Core::IndexType>().GetBuffer(), 0,
                         VK_INDEX_TYPE_UINT32);
    Material::CAbstractMaterial* currentMaterial = nullptr;
    for (auto& [material, instanceRanges] : mMeshInstanceRanges) {
        currentMaterial = material;
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

        for (const auto& range : instanceRanges) {
            vkCmdDrawIndexedIndirect(
                commandBuffer,
                mBufferHandler.Get<Core::SIndirectDrawCommand>().GetBuffer(),
                range.first * sizeof(Core::SIndirectDrawCommand), range.count,
                sizeof(Core::SIndirectDrawCommand));
        }
    }
}

} // namespace Renderer