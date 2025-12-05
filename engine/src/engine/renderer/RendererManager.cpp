#include "engine/renderer/RendererManager.h"

#include "engine/core/Camera.h"
#include "engine/material/MaterialManager.h"
#include "engine/vulkan/BufferHandler.h"

namespace Renderer {
CRendererManager::CRendererManager(Vulkan::CBufferHandler& bufferHandler,
                                   Material::CMaterialManager& materialManager)
    : mBufferHandler(bufferHandler)
    , mMaterialManager(materialManager)
    , mMeshRenderer(mBufferHandler.GetWrapper<Core::SVertex>(),
                    mBufferHandler.GetWrapper<Core::IndexType>(),
                    mBufferHandler.GetWrapper<Core::SInstanceData>(),
                    mBufferHandler.GetWrapper<Core::SIndirectDrawCommand>())
    , mTextRenderer(mBufferHandler.GetWrapper<Core::SVertex>(),
                    mBufferHandler.GetWrapper<Core::IndexType>(),
                    mBufferHandler.GetWrapper<Core::STextInstanceData>(),
                    mBufferHandler.GetWrapper<Core::SIndirectDrawCommand>()) {
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
    mBufferHandler.Free();
}

void CRendererManager::UploadSceneData() {
    mTextRenderer.Prepare();
    mMeshRenderer.Prepare();
    mBufferHandler.Upload<Core::SVertex>();
    mBufferHandler.Upload<Core::IndexType>();
}

void CRendererManager::RebuildInstances(
    const std::vector<SRenderable>& renderables) {
    mBufferHandler.FreeBuffer<Core::SInstanceData>();
    mBufferHandler.FreeBuffer<Core::SIndirectDrawCommand>();
    mMeshInstanceRanges = mMeshRenderer.RebuildInstances(renderables);
    // mTextRenderer.RebuildInstances(renderables);

    mBufferHandler.Upload<Core::SInstanceData>();
    mBufferHandler.Upload<Core::SIndirectDrawCommand>();
}

void CRendererManager::Render(VkCommandBuffer commandBuffer,
                              VkDescriptorSet descriptorSet,
                              Core::CCamera& camera) {
    VkDeviceSize offsets[] = {0, 0};
    VkBuffer vertexBuffers[] = {
        mBufferHandler.GetWrapper<Core::SVertex>().GetBuffer()->GetBuffer(),
        mBufferHandler.GetWrapper<Core::SInstanceData>()
            .GetBuffer()
            ->GetBuffer()};

    vkCmdBindVertexBuffers(commandBuffer, 0, 2, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(
        commandBuffer,
        mBufferHandler.GetWrapper<Core::IndexType>().GetBuffer()->GetBuffer(),
        0, VK_INDEX_TYPE_UINT32);
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
                mBufferHandler.GetWrapper<Core::SIndirectDrawCommand>()
                    .GetBuffer()
                    ->GetBuffer(),
                range.first * sizeof(Core::SIndirectDrawCommand), range.count,
                sizeof(Core::SIndirectDrawCommand));
        }
    }
}

} // namespace Renderer