#include "engine/material/AbstractMaterial.h"

#include "engine/vulkan/DescriptorStorage.h"

namespace {
static std::size_t gMaterialIdCounter = 0;
} // namespace

namespace Material {
CAbstractMaterial::CAbstractMaterial(EMaterialType type,
                                     Renderer::EVertexLayout vertexLayout,
                                     Renderer::SPipelineDescriptors& pipeline)
    : mType(type)
    , mVertexLayout(vertexLayout)
    , mPipeline(pipeline)
    , mId(gMaterialIdCounter++) {
}

CAbstractMaterial::CAbstractMaterial(const CAbstractMaterial& other)
    : mType(other.mType)
    , mVertexLayout(other.mVertexLayout)
    , mPipeline(other.mPipeline)
    , mColor(other.mColor)
    , mTextureIndex(other.mTextureIndex)
    , mId(gMaterialIdCounter++) {
}

CAbstractMaterial::CAbstractMaterial(CAbstractMaterial&& other) noexcept
    : mType(other.mType)
    , mVertexLayout(other.mVertexLayout)
    , mPipeline(other.mPipeline)
    , mColor(other.mColor)
    , mTextureIndex(other.mTextureIndex)
    , mId(gMaterialIdCounter++) {
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

std::size_t CAbstractMaterial::GetId() const {
    return mId;
}

void CAbstractMaterial::Bind(VkDevice device, VkCommandBuffer commandBuffer,
                             SMaterialBindingInfo& bindingInfo,
                             VkDescriptorPool descriptorPool) {
}

} // namespace Material