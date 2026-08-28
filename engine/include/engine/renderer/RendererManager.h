// #pragma once

// #include "engine/renderer/MaterialStructures.h"

// namespace Core {
// class CCamera;
// } // namespace Core

// namespace Vulkan {
// class CBufferHandler;
// class CPipelineFactory;
// } // namespace Vulkan

// namespace Renderer {
// class CTransformManager;
// struct SPipelineDescriptors;

// class CRendererManager {
// public:
//     CRendererManager(Vulkan::CBufferHandler& bufferHandler,
//                      Vulkan::CPipelineFactory& pipelineFactory);
//     ~CRendererManager();

//     void FreeSceneData();
//     void Prepare();

//     void GenerateInstances();
//     void Render(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet,
//                 Core::CCamera& camera, Core::CCamera& uiCamera);

// private:
//     Vulkan::CBufferHandler& mBufferHandler;
//     Vulkan::CPipelineFactory& mPipelineFactory;

//     Renderer::SPipelineDescriptors mCullPipeline;
// };
// } // namespace Renderer
