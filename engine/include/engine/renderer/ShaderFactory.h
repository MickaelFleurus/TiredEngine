#pragma once

#include <memory>
#include <string>

#include "engine/renderer/MaterialStructures.h"
#include <vulkan/vulkan.h>

namespace Renderer {
class CWindow;
class CDescriptorLayoutStorage;

class CShaderFactory {
public:
    CShaderFactory(const CWindow& window);
    ~CShaderFactory();

    SShaderDescriptors
    CreateFragmentShader(std::string name, std::string path,
                         CDescriptorLayoutStorage& descriptorLayoutStorage);
    SShaderDescriptors
    CreateVertexShader(std::string name, std::string path,
                       CDescriptorLayoutStorage& descriptorLayoutStorage);

private:
    class CImpl;
    std::unique_ptr<CImpl> mImpl;
};
} // namespace Renderer