#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace Vulkan {
class CVulkanContext;
class CDescriptorStorage {
public:
    CDescriptorStorage(const Vulkan::CVulkanContext& contextGetter);
    ~CDescriptorStorage();

    void Init();

    const std::vector<VkDescriptorSetLayoutBinding>& GetBindingInfo() const;
    VkDescriptorSetLayout GetLayout() const;
    VkDescriptorSet GetDescriptorSet() const;
    VkDescriptorPool GetDescriptorPool() const;

private:
    const Vulkan::CVulkanContext& mContextGetter;
    VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout mLayout = VK_NULL_HANDLE;
    VkDescriptorSet mDescriptorSet = VK_NULL_HANDLE;
    std::vector<VkDescriptorSetLayoutBinding> mBindingInfos;
};
} // namespace Vulkan