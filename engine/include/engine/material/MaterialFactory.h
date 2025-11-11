#pragma once

#include "engine/material/MaterialTypes.h"
#include "engine/renderer/PipelineFactory.h"

#include <memory>
#include <string>

struct SDL_GPUTexture;
struct SDL_GPUDevice;
struct SDL_Window;

namespace Renderer {
struct SPipelineConfig;
class CTextureManager;
class CWindow;
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
                              const Renderer::CWindow& window);

    // Create material with specified parameters
    std::unique_ptr<AbstractMaterial>
    CreateMaterial(EMaterialType type, const Renderer::SPipelineConfig& info);

    std::unique_ptr<AbstractMaterial>
    CreateTextMaterial(std::string fontAtlasName);
    // // Pre-defined material types
    // std::unique_ptr<AbstractMaterial>
    // CreateUnlitMaterial(SDL_GPUTexture* texture);
    // std::unique_ptr<AbstractMaterial>
    // CreateSpriteMaterial(SDL_GPUTexture* sprite);

private:
    Renderer::CTextureManager& mTextureManager;
    Utils::CFileHandler& mFileHandler;
    const Renderer::CWindow& mWindow;
    Renderer::CPipelineFactory mPipelineFactory;
};

} // namespace Material