#include "engine/material/AbstractMaterial.h"

#include "engine/renderer/DescriptorLayoutStorage.h"

namespace Material {
AbstractMaterial::AbstractMaterial(
    const Renderer::CDescriptorLayoutStorage& descriptorLayoutStorage,
    EMaterialType type, Renderer::EVertexLayout vertexLayout,
    Renderer::SPipelineDescriptors& pipeline)
    : mDescriptorLayoutStorage(descriptorLayoutStorage)
    , mType(type)
    , mVertexLayout(vertexLayout)
    , mPipeline(pipeline) {
}

AbstractMaterial::AbstractMaterial(const AbstractMaterial& other)
    : mDescriptorLayoutStorage(other.mDescriptorLayoutStorage)
    , mType(other.mType)
    , mVertexLayout(other.mVertexLayout)
    , mPipeline(other.mPipeline)
    , mColor(other.mColor)
    , mTexture(other.mTexture) {
}

AbstractMaterial::AbstractMaterial(AbstractMaterial&& other) noexcept
    : mDescriptorLayoutStorage(other.mDescriptorLayoutStorage)
    , mType(other.mType)
    , mVertexLayout(other.mVertexLayout)
    , mPipeline(other.mPipeline)
    , mColor(other.mColor)
    , mTexture(other.mTexture) {
    other.mPipeline = {};
    other.mTexture = nullptr;
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

void AbstractMaterial::SetTexture(SDL_GPUTexture* texture) {
    mTexture = texture;
}

const glm::vec4& AbstractMaterial::GetColor() const {
    return mColor;
}

SDL_GPUTexture* AbstractMaterial::GetTexture() const {
    return mTexture;
}

void AbstractMaterial::Bind(VkDevice device, VkCommandBuffer commandBuffer,
                            SMaterialBindingInfo& bindingInfo,
                            VkDescriptorPool descriptorPool) {
    VkDeviceSize offsets[] = {0, 0};

    vkCmdBindVertexBuffers(
        commandBuffer, 0,
        static_cast<uint32_t>(bindingInfo.bufferBindings.size()),
        bindingInfo.bufferBindings.data(), offsets);

    for (size_t hash : mPipeline.descriptorSetLayoutsHashes) {
        const auto& bindings = mDescriptorLayoutStorage.GetBindingInfo(hash);
        VkDescriptorSetLayout layout = mDescriptorLayoutStorage.GetLayout(hash);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;
        VkDescriptorSet descriptorSet;
        vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);

        std::vector<VkWriteDescriptorSet> writes;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkDescriptorImageInfo> imageInfos;

        for (const auto& binding : bindings) {
            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = descriptorSet;
            write.dstBinding = binding.binding;
            write.descriptorType = binding.descriptorType;
            write.descriptorCount = binding.descriptorCount;

            if (binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                write.pBufferInfo = &bindingInfo.uniformBuffers[0]; // Example
            } else if (binding.descriptorType ==
                       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
                write.pImageInfo = &bindingInfo.imageViews[0]; // Example
            }

            writes.push_back(write);
        }

        // 4. Update the descriptor set
        vkUpdateDescriptorSets(device, static_cast<uint32_t>(writes.size()),
                               writes.data(), 0, nullptr);
    }
}

} // namespace Material