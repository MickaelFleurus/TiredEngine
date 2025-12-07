#pragma once
#include "engine/material/MaterialTypes.h"
#include "engine/renderer/MaterialStructures.h"
#include "engine/renderer/PipelineTypes.h"

#include <glm/vec4.hpp>
#include <vulkan/vulkan.h>

namespace Material {

class CAbstractMaterial {
public:
    virtual ~CAbstractMaterial() = default;

    EMaterialType GetType() const;

    void SetColor(const glm::vec4& color);
    void SetTextureIndex(int textureIndex);

    const glm::vec4& GetColor() const;
    int GetTextureIndex() const;

    VkPipeline GetPipeline() const;
    VkPipelineLayout GetPipelineLayout() const;

    std::size_t GetId() const;

    virtual void Bind(VkDevice device, VkCommandBuffer commandBuffer,
                      SMaterialBindingInfo& bindingInfo,
                      VkDescriptorPool descriptorPool);

protected:
    explicit CAbstractMaterial(EMaterialType type,
                               Renderer::EVertexLayout vertexLayout,
                               Renderer::SPipelineDescriptors& pipeline);
    CAbstractMaterial(const CAbstractMaterial& other);
    CAbstractMaterial& operator=(const CAbstractMaterial& other) = delete;
    CAbstractMaterial(CAbstractMaterial&& other) noexcept;
    CAbstractMaterial& operator=(CAbstractMaterial&& other) noexcept = delete;

    Renderer::EVertexLayout mVertexLayout;
    EMaterialType mType = EMaterialType::Unlit;
    Renderer::SPipelineDescriptors mPipeline;
    glm::vec4 mColor = glm::vec4(1.0f);
    int mTextureIndex = -1;
    const std::size_t mId;
};

} // namespace Material