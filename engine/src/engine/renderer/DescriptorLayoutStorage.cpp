#include "engine/renderer/DescriptorLayoutStorage.h"

#include "engine/renderer/VulkanRenderer.h"
#include "engine/renderer/Window.h"
#include "engine/utils/Logger.h"
#include <spirv_reflect.h>

namespace {
VkDescriptorType ToVkDescriptorType(SpvReflectDescriptorType type) {
    switch (type) {
    case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
        return VK_DESCRIPTOR_TYPE_SAMPLER;
    case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
        return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
        return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
        return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
    case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
        return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    default:
        return VK_DESCRIPTOR_TYPE_MAX_ENUM;
    }
}

size_t HashBinding(const std::vector<VkDescriptorSetLayoutBinding>& bindings) {
    size_t seed = bindings.size();
    for (const auto& b : bindings) {
        // Combine fields into the hash
        seed ^= std::hash<uint32_t>()(b.binding) + 0x9e3779b9 + (seed << 6) +
                (seed >> 2);
        seed ^= std::hash<uint32_t>()(b.descriptorType) + 0x9e3779b9 +
                (seed << 6) + (seed >> 2);
        seed ^= std::hash<uint32_t>()(b.descriptorCount) + 0x9e3779b9 +
                (seed << 6) + (seed >> 2);
        seed ^= std::hash<uint32_t>()(b.stageFlags) + 0x9e3779b9 + (seed << 6) +
                (seed >> 2);
    }
    return seed;
}
} // namespace

namespace Renderer {

CDescriptorLayoutStorage::CDescriptorLayoutStorage(const CWindow& window)
    : mWindow(window) {
}

CDescriptorLayoutStorage::~CDescriptorLayoutStorage() {
    for (auto& [hash, layout] : mLayouts) {
        vkDestroyDescriptorSetLayout(mWindow.GetVulkanRenderer().GetDevice(),
                                     layout, nullptr);
    }
}

std::vector<size_t>
CDescriptorLayoutStorage::ReflectOnShader(uint32_t codeSize, void* rawCode,
                                          const std::string& path) {
    SpvReflectShaderModule module;
    SpvReflectResult result =
        spvReflectCreateShaderModule(codeSize, rawCode, &module);
    if (result != SPV_REFLECT_RESULT_SUCCESS) {
        LOG_ERROR("Failed to reflect shader module: {}", path);
        return {};
    }
    uint32_t set_count = 0;
    spvReflectEnumerateDescriptorSets(&module, &set_count, NULL);
    std::vector<SpvReflectDescriptorSet*> sets(set_count);
    spvReflectEnumerateDescriptorSets(&module, &set_count, sets.data());

    std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>>
        set_bindings;

    for (const auto* set : sets) {
        for (uint32_t i = 0; i < set->binding_count; ++i) {
            const SpvReflectDescriptorBinding* binding = set->bindings[i];
            VkDescriptorSetLayoutBinding vk_binding{};
            vk_binding.binding = binding->binding;
            vk_binding.descriptorType =
                ToVkDescriptorType(binding->descriptor_type);
            vk_binding.descriptorCount = binding->count;
            vk_binding.stageFlags =
                static_cast<VkShaderStageFlags>(module.shader_stage);
            vk_binding.pImmutableSamplers = nullptr;
            set_bindings[set->set].push_back(vk_binding);
        }
    }

    std::vector<size_t> layoutIndices;
    if (!set_bindings.empty()) {
        VkDevice device = mWindow.GetVulkanRenderer().GetDevice();
        for (const auto& [set_num, bindings] : set_bindings) {
            size_t hash = HashBinding(bindings);
            if (!mLayouts.contains(hash)) {
                VkDescriptorSetLayoutCreateInfo layout_info{};
                layout_info.sType =
                    VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                layout_info.bindingCount =
                    static_cast<uint32_t>(bindings.size());
                layout_info.pBindings = bindings.data();
                VkDescriptorSetLayout layout;
                VkResult res = vkCreateDescriptorSetLayout(device, &layout_info,
                                                           nullptr, &layout);
                if (res != VK_SUCCESS) {
                    LOG_ERROR(
                        "Failed to create descriptor set layout for set {}",
                        set_num);
                    return {};
                }
                mLayouts[hash] = layout;
                mBindingInfos[hash] = bindings;
            }
            layoutIndices.push_back(hash);
        }
    }
    spvReflectDestroyShaderModule(&module);
    return layoutIndices;
}

const std::vector<VkDescriptorSetLayoutBinding>&
CDescriptorLayoutStorage::GetBindingInfo(size_t hash) const {
    return mBindingInfos.at(hash);
}

VkDescriptorSetLayout CDescriptorLayoutStorage::GetLayout(size_t hash) const {
    return mLayouts.at(hash);
}

} // namespace Renderer