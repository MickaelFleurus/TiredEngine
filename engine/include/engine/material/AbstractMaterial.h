#pragma once
#include "engine/material/MaterialTypes.h"
#include "engine/renderer/MaterialStructures.h"
#include "engine/renderer/PipelineTypes.h"

#include <glm/vec4.hpp>
#include <vulkan/vulkan.h>

namespace Material {

class AbstractMaterial {
public:
    virtual ~AbstractMaterial() = default;

    EMaterialType GetType() const;
    VkPipeline GetPipeline() const;

    void SetColor(const glm::vec4& color);
    void SetTextureIndex(int textureIndex);

    const glm::vec4& GetColor() const;
    int GetTextureIndex() const;
    VkPipelineLayout GetPipelineLayout() const;

    virtual void Bind(VkDevice device, VkCommandBuffer commandBuffer,
                      SMaterialBindingInfo& bindingInfo,
                      VkDescriptorPool descriptorPool);

protected:
    explicit AbstractMaterial(EMaterialType type,
                              Renderer::EVertexLayout vertexLayout,
                              Renderer::SPipelineDescriptors& pipeline);
    AbstractMaterial(const AbstractMaterial& other);
    AbstractMaterial& operator=(const AbstractMaterial& other) = delete;
    AbstractMaterial(AbstractMaterial&& other) noexcept;
    AbstractMaterial& operator=(AbstractMaterial&& other) noexcept = delete;

    Renderer::EVertexLayout mVertexLayout;
    EMaterialType mType = EMaterialType::Unlit;
    Renderer::SPipelineDescriptors mPipeline;
    glm::vec4 mColor = glm::vec4(1.0f);
    int mTextureIndex = -1;
};

} // namespace Material