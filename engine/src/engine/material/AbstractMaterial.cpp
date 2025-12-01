#include "engine/material/AbstractMaterial.h"

#include "engine/vulkan/DescriptorStorage.h"

namespace Material {
CAbstractMaterial::CAbstractMaterial(EMaterialType type,
                                     Renderer::EVertexLayout vertexLayout,
                                     Renderer::SPipelineDescriptors& pipeline)
    : mType(type), mVertexLayout(vertexLayout), mPipeline(pipeline) {
}

CAbstractMaterial::CAbstractMaterial(const CAbstractMaterial& other)
    : mType(other.mType)
    , mVertexLayout(other.mVertexLayout)
    , mPipeline(other.mPipeline)
    , mColor(other.mColor)
    , mTextureIndex(other.mTextureIndex) {
}

CAbstractMaterial::CAbstractMaterial(CAbstractMaterial&& other) noexcept
    : mType(other.mType)
    , mVertexLayout(other.mVertexLayout)
    , mPipeline(other.mPipeline)
    , mColor(other.mColor)
    , mTextureIndex(other.mTextureIndex) {
    other.mPipeline = {};
    other.mTextureIndex = -1;
}

EMaterialType CAbstractMaterial::GetType() const {
    return mType;
}

VkPipeline CAbstractMaterial::GetPipeline() const {
    return mPipeline.pipeline;
}

void CAbstractMaterial::SetColor(const glm::vec4& color) {
    mColor = color;
}

void CAbstractMaterial::SetTextureIndex(int textureIndex) {
    mTextureIndex = textureIndex;
}

const glm::vec4& CAbstractMaterial::GetColor() const {
    return mColor;
}

int CAbstractMaterial::GetTextureIndex() const {
    return mTextureIndex;
}

VkPipelineLayout CAbstractMaterial::GetPipelineLayout() const {
    return mPipeline.pipelineLayout;
}

void CAbstractMaterial::Bind(VkDevice device, VkCommandBuffer commandBuffer,
                             SMaterialBindingInfo& bindingInfo,
                             VkDescriptorPool descriptorPool) {
}

} // namespace Material