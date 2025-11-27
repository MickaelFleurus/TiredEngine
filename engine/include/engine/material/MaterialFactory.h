#pragma once

#include "engine/material/MaterialTypes.h"
#include "engine/renderer/PipelineFactory.h"

#include <memory>
#include <string>

namespace Vulkan {
class CVulkanContext;
} // namespace Vulkan

namespace Renderer {
struct SPipelineConfig;
class CTextureManager;
class CDescriptorStorage;
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
                              Renderer::CDescriptorStorage& descriptorStorage);

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
    Renderer::CPipelineFactory mPipelineFactory;
    Renderer::CDescriptorStorage& mDescriptorStorage;
};

} // namespace Material