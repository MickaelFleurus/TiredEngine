#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace Vulkan {
struct SContext;
class CDescriptorStorage {
public:
    CDescriptorStorage(const Vulkan::SContext& context);
    ~CDescriptorStorage();

    void Init();

    const std::vector<VkDescriptorSetLayoutBinding>& GetBindingInfo() const;
    VkDescriptorSetLayout GetLayout() const;
    VkDescriptorSet GetDescriptorSet() const;
    VkDescriptorPool GetDescriptorPool() const;

private:
    const Vulkan::SContext& mContext;
    VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout mLayout = VK_NULL_HANDLE;
    VkDescriptorSet mDescriptorSet = VK_NULL_HANDLE;
    std::vector<VkDescriptorSetLayoutBinding> mBindingInfos;
};
} // namespace Vulkan
