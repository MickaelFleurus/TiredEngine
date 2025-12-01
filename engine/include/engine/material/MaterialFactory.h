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
class AbstractMaterial;

class CMaterialFactory {
public:
    explicit CMaterialFactory(Renderer::CTextureManager& textureManager,
                              Utils::CFileHandler& fileHandler,
                              const Vulkan::CVulkanContext& contextGetter,
                              Vulkan::CDescriptorStorage& descriptorStorage);

    // Create material with specified parameters
    std::unique_ptr<AbstractMaterial>
    CreateMaterial(EMaterialType type, const Renderer::SPipelineConfig& info);

    std::unique_ptr<AbstractMaterial> GetOrCreateTextMaterial();
    // // Pre-defined material types
    // std::unique_ptr<AbstractMaterial>
    // CreateUnlitMaterial(SDL_GPUTexture* texture);
    // std::unique_ptr<AbstractMaterial>
    // CreateSpriteMaterial(SDL_GPUTexture* sprite);

private:
    Renderer::CTextureManager& mTextureManager;
    Utils::CFileHandler& mFileHandler;
    Vulkan::CPipelineFactory mPipelineFactory;
    Vulkan::CDescriptorStorage& mDescriptorStorage;
};

} // namespace Material