#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>

namespace Renderer {
class CWindow;
class CDescriptorLayoutStorage {
public:
    CDescriptorLayoutStorage(const CWindow& window);
    ~CDescriptorLayoutStorage();

    std::vector<size_t> ReflectOnShader(uint32_t codeSize, void* rawCode,
                                        const std::string& path);

    const std::vector<VkDescriptorSetLayoutBinding>&
    GetBindingInfo(size_t hash) const;
    VkDescriptorSetLayout GetLayout(size_t hash) const;

private:
    const CWindow& mWindow;
    std::unordered_map<size_t, VkDescriptorSetLayout> mLayouts;
    std::unordered_map<size_t, std::vector<VkDescriptorSetLayoutBinding>>
        mBindingInfos;
};
} // namespace Renderer