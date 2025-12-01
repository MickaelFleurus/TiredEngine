#pragma once

#include "engine/material/MaterialTypes.h"
#include "engine/vulkan/PipelineFactory.h"

#include <memory>
#include <string>

namespace Vulkan {
class CVulkanContext;
class CDescriptorStorage;
} // namespace Vulkan

namespace Renderer {
struct SPipelineConfig;
class CTextureManager;
} // namespace Renderer

namespace Utils {
class CFileHandler;
}

namespace Material {
class CAbstractMaterial;

class CMaterialFactory {
public:
    explicit CMaterialFactory(Renderer::CTextureManager& textureManager,
                              Utils::CFileHandler& fileHandler,
                              const Vulkan::CVulkanContext& contextGetter,
                              Vulkan::CDescriptorStorage& descriptorStorage);
    ~CMaterialFactory();

    std::unique_ptr<CAbstractMaterial> GetMaterial(EMaterialType type);

    // // Pre-defined material types
    // std::unique_ptr<CAbstractMaterial>
    // CreateUnlitMaterial(SDL_GPUTexture* texture);
    // std::unique_ptr<CAbstractMaterial>
    // CreateSpriteMaterial(SDL_GPUTexture* sprite);

private:
    std::unique_ptr<CAbstractMaterial>
    CreateMaterial(EMaterialType type, const Renderer::SPipelineConfig& info);

    std::unique_ptr<CAbstractMaterial> CreateTextMaterial();

    Renderer::CTextureManager& mTextureManager;
    Utils::CFileHandler& mFileHandler;
    Vulkan::CPipelineFactory mPipelineFactory;
    Vulkan::CDescriptorStorage& mDescriptorStorage;
};

} // namespace Material