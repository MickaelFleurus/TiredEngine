#include "engine/renderer/RendererManager.h"

#include "engine/component/ComponentManager.h"
#include "engine/core/Camera.h"
#include "engine/material/MaterialManager.h"
#include "engine/renderer/PipelineTypes.h"
#include "engine/vulkan/BufferHandler.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace {
const Renderer::SComputePipelineConfig kCullPipelineConfig{
    .shaderName = "CullPipeline", .shaderPath = "shaders/CullShader.comp"};
}

namespace Renderer {

CRendererManager::CRendererManager(Vulkan::CBufferHandler& bufferHandler,
                                   Material::CMaterialManager& materialManager,
                                   CTransformManager& transformManager,
                                   Vulkan::CPipelineFactory& pipelineFactory)
    : mBufferHandler(bufferHandler)
    , mMaterialManager(materialManager)
    , mTransformManager(transformManager)
    , mMeshRenderer(mTransformManager)
    , mPipelineFactory(pipelineFactory)
    , mCullPipeline(
          pipelineFactory.GetOrCreateComputePipeline(kCullPipelineConfig)) {
}

CRendererManager::~CRendererManager() = default;

void CRendererManager::Prepare() {
}

CMeshRenderer& CRendererManager::GetMeshRenderer() {
    return mMeshRenderer;
}

void CRendererManager::FreeSceneData() {
    mMeshRenderer.Free();
}

void CRendererManager::GenerateInstances(
    Component::CComponentManager& componentManager) {
    // mMeshRenderer.UpdateInstances(
    //     componentManager.GetComponents(Component::EComponentType::Mesh));
}

void CRendererManager::Render(VkCommandBuffer commandBuffer,
                              VkDescriptorSet descriptorSet,
                              Core::CCamera& camera, Core::CCamera& uiCamera) {
    auto indirectDrawBuffers = mBufferHandler.GetIndirectDrawBuffers();
    indirectDrawBuffers.ResetDrawGroupCounts(commandBuffer);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                      mCullPipeline.pipeline);
    for (auto& group : indirectDrawBuffers.GetDrawGroups()) {
        Core::SCullPushConstants pc{
            meshInfoBuffer.deviceAddress, group.indirectBuffer.deviceAddress,
            group.countBuffer.deviceAddress, group.meshCount, frustumPlanes};
        vkCmdPushConstants(commandBuffer, mCullPipeline.pipelineLayout,
                           VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(pc), &pc);
        vkCmdDispatch(commandBuffer, (group.meshCount + 63) / 64, 1, 1);
    }

    // single barrier covers all groups' writes if they share stage/access masks
    // — one call is enough
    vkCmdPipelineBarrier2(commandBuffer, &depInfo);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            mCullPipeline.pipelineLayout, 0, 1,
                            &bindlessTextureSet, 0, nullptr);
    for (auto& group : indirectDrawBuffers.GetDrawGroups()) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          group.pipeline);
        // vkCmdPushConstants(commandBuffer, pipelineLayout, ...,
        //                    &sceneBufferAddresses);
        vkCmdDrawIndexedIndirectCount(
            commandBuffer, group.indirectBuffer.buffer, 0,
            group.countBuffer.buffer, 0, group.maxDrawCount,
            sizeof(Core::SIndirectDrawCommand));
    }
}

} // namespace Renderer
