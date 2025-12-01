#include "engine/vulkan/DescriptorStorage.h"

#include "engine/utils/Logger.h"
#include "engine/vulkan/VulkanContext.h"

#include "engine/vulkan/Constants.h"

namespace Vulkan {

CDescriptorStorage::CDescriptorStorage(
    const Vulkan::CVulkanContext& contextGetter)
    : mContextGetter(contextGetter) {
    Init();
}

CDescriptorStorage::~CDescriptorStorage() {
    vkDestroyDescriptorSetLayout(mContextGetter.GetDevice(), mLayout, nullptr);
    vkDestroyDescriptorPool(mContextGetter.GetDevice(), mDescriptorPool,
                            nullptr);
}

void CDescriptorStorage::Init() {

    VkDescriptorPoolSize poolSizes[] = {
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, kMaxTextures},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, kMaxUBO},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, kMaxSSBO}
        // Add more types as needed
    };

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 3;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = kMaxTextures + kMaxUBO + kMaxSSBO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

    if (vkCreateDescriptorPool(mContextGetter.GetDevice(), &poolInfo, nullptr,
                               &mDescriptorPool) != VK_SUCCESS) {
        LOG_FATAL("Failed to create texture descriptor pool!");
    }

    VkDescriptorSetLayoutBinding textureBinding{};
    textureBinding.binding = kTextureBinding;
    textureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    textureBinding.descriptorCount = Vulkan::kMaxTextures;
    textureBinding.stageFlags =
        VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;
    textureBinding.pImmutableSamplers = nullptr;
    mBindingInfos.push_back(textureBinding);

    VkDescriptorSetLayoutBinding instancesBinding{};
    instancesBinding.binding = kInstanceBufferBinding;
    instancesBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    instancesBinding.descriptorCount = Vulkan::kInstanceAmountPerBuffer;
    instancesBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    instancesBinding.pImmutableSamplers = nullptr;
    mBindingInfos.push_back(instancesBinding);

    VkDescriptorBindingFlags bindingFlag =
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
        VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
    VkDescriptorBindingFlags bindingFlags[2] = {bindingFlag, bindingFlag};

    VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsInfo{};
    bindingFlagsInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    bindingFlagsInfo.bindingCount = 2;
    bindingFlagsInfo.pBindingFlags = bindingFlags;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(mBindingInfos.size());
    layoutInfo.pBindings = mBindingInfos.data();
    layoutInfo.pNext = &bindingFlagsInfo;
    layoutInfo.flags =
        VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

    if (vkCreateDescriptorSetLayout(mContextGetter.GetDevice(), &layoutInfo,
                                    nullptr, &mLayout) != VK_SUCCESS) {
        LOG_ERROR("Failed to create descriptor set layout!");
        return;
    }

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = mDescriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &mLayout;

    if (vkAllocateDescriptorSets(mContextGetter.GetDevice(), &allocInfo,
                                 &mDescriptorSet) != VK_SUCCESS) {
        LOG_FATAL("Failed to allocate texture descriptor set!");
    }
}

std::vector<VkDescriptorSetLayoutBinding> const&
CDescriptorStorage::GetBindingInfo() const {
    return mBindingInfos;
}

VkDescriptorSetLayout CDescriptorStorage::GetLayout() const {
    return mLayout;
}

VkDescriptorSet CDescriptorStorage::GetDescriptorSet() const {
    return mDescriptorSet;
}

VkDescriptorPool CDescriptorStorage::GetDescriptorPool() const {
    return mDescriptorPool;
}

} // namespace Vulkan