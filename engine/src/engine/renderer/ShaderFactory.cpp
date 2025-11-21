#include "engine/renderer/ShaderFactory.h"

#include "engine/renderer/DescriptorLayoutStorage.h"
#include "engine/renderer/VulkanRenderer.h"
#include "engine/renderer/Window.h"
#include "engine/utils/Logger.h"

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_iostream.h>
#include <vulkan/vulkan.h>

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace Renderer {

class CShaderFactory::CImpl {
public:
    CImpl(const CWindow& window) : mWindow(window) {
    }

    ~CImpl() {
        for (auto& [path, shader] : mShaderCache) {
            vkDestroyShaderModule(mWindow.GetVulkanRenderer().GetDevice(),
                                  shader, nullptr);
        }
    }

    SShaderDescriptors
    CreateShader(std::string name, std::string path,
                 CDescriptorLayoutStorage& descriptorLayoutStorage) {
        path += name + ".spv";
        std::vector<size_t> descriptorSetLayouts;
        if (!mShaderCache.contains(name)) {
            size_t codeSize = 0;
            std::unique_ptr<void, decltype(&SDL_free)> rawCode =
                std::unique_ptr<void, decltype(&SDL_free)>(
                    SDL_LoadFile(path.c_str(), &codeSize), SDL_free);
            if (rawCode == nullptr) {
                LOG_ERROR("Failed to load shader file: {}", path);
                return {};
            }

            descriptorSetLayouts = descriptorLayoutStorage.ReflectOnShader(
                static_cast<uint32_t>(codeSize), rawCode.get(), path);

            VkShaderModuleCreateInfo shaderModuleCreateInfo{};
            shaderModuleCreateInfo.sType =
                VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            shaderModuleCreateInfo.codeSize = codeSize;
            shaderModuleCreateInfo.pCode =
                reinterpret_cast<const uint32_t*>(rawCode.get());

            auto device = mWindow.GetVulkanRenderer().GetDevice();
            VkShaderModule shaderModule;
            if (vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr,
                                     &shaderModule) == VK_SUCCESS) {
                mShaderCache.emplace(name, shaderModule);
            } else {
                LOG_ERROR("Failed to create shader module for shader: {}",
                          name);
                return {};
            }
        }
        return {mShaderCache[name], descriptorSetLayouts};
    }

private:
    const CWindow& mWindow;
    std::unordered_map<std::string, VkShaderModule> mShaderCache;
};

CShaderFactory::CShaderFactory(const CWindow& window)
    : mImpl(std::make_unique<CImpl>(window)) {
}

CShaderFactory::~CShaderFactory() = default;

SShaderDescriptors CShaderFactory::CreateFragmentShader(
    std::string name, std::string path,
    CDescriptorLayoutStorage& descriptorLayoutStorage) {
    return mImpl->CreateShader(name + ".frag", path, descriptorLayoutStorage);
}

SShaderDescriptors CShaderFactory::CreateVertexShader(
    std::string name, std::string path,
    CDescriptorLayoutStorage& descriptorLayoutStorage) {
    return mImpl->CreateShader(name + ".vert", path, descriptorLayoutStorage);
}

} // namespace Renderer