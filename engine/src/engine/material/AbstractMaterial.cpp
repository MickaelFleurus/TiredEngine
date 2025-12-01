#include "engine/material/AbstractMaterial.h"

#include "engine/vulkan/DescriptorStorage.h"

namespace Material {
AbstractMaterial::AbstractMaterial(EMaterialType type,
                                   Renderer::EVertexLayout vertexLayout,
                                   Renderer::SPipelineDescriptors& pipeline)
    : mType(type), mVertexLayout(vertexLayout), mPipeline(pipeline) {
}

AbstractMaterial::AbstractMaterial(const AbstractMaterial& other)
    : mType(other.mType)
    , mVertexLayout(other.mVertexLayout)
    , mPipeline(other.mPipeline)
    , mColor(other.mColor)
    , mTextureIndex(other.mTextureIndex) {
}

AbstractMaterial::AbstractMaterial(AbstractMaterial&& other) noexcept
    : mType(other.mType)
    , mVertexLayout(other.mVertexLayout)
    , mPipeline(other.mPipeline)
    , mColor(other.mColor)
    , mTextureIndex(other.mTextureIndex) {
    other.mPipeline = {};
    other.mTextureIndex = -1;
}

EMaterialType AbstractMaterial::GetType() const {
    return mType;
}

VkPipeline AbstractMaterial::GetPipeline() const {
    return mPipeline.pipeline;
}

void AbstractMaterial::SetColor(const glm::vec4& color) {
    mColor = color;
}

void AbstractMaterial::SetTextureIndex(int textureIndex) {
    mTextureIndex = textureIndex;
}

const glm::vec4& AbstractMaterial::GetColor() const {
    return mColor;
}

int AbstractMaterial::GetTextureIndex() const {
    return mTextureIndex;
}

VkPipelineLayout AbstractMaterial::GetPipelineLayout() const {
    return mPipeline.pipelineLayout;
}

void AbstractMaterial::Bind(VkDevice device, VkCommandBuffer commandBuffer,
                            SMaterialBindingInfo& bindingInfo,
                            VkDescriptorPool descriptorPool) {
}

} // namespace Material