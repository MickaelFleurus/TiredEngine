#include "engine/material/MaterialFactory.h"

#include "engine/material/Material.h"
#include "engine/renderer/PipelineTypes.h"
#include "engine/renderer/TextureManager.h"

#include "engine/utils/FileHandler.h"

namespace Material {

CMaterialFactory::CMaterialFactory(Renderer::CTextureManager& textureManager,
                                   Utils::CFileHandler& fileHandler,
                                   const Renderer::CWindow& window)
    : mTextureManager(textureManager)
    , mFileHandler(fileHandler)
    , mWindow(window)
    , mPipelineFactory(window) {
}

std::unique_ptr<AbstractMaterial>
CMaterialFactory::CreateMaterial(EMaterialType type,
                                 const Renderer::SPipelineConfig& info) {

    // Get or create pipeline
    SDL_GPUGraphicsPipeline* pipeline =
        mPipelineFactory.CreateGraphicsPipeline(info);
    return std::make_unique<CMaterial>(type, pipeline);
}

std::unique_ptr<AbstractMaterial>
CMaterialFactory::CreateTextMaterial(std::string fontAtlasName) {
    Renderer::SPipelineConfig config;
    config.shaderName = "TextShader";
    config.shaderPath = mFileHandler.GetAssetsFolder() + "shaders/";
    config.vertexLayout = Renderer::EVertexLayout::Instanced;
    config.enableBlending = true;

    config.vertexResources.numUniformBuffers = 1;
    config.fragmentResources.numSamplers = 1;

    auto material = CreateMaterial(EMaterialType::Text, config);
    material->SetTexture(mTextureManager.GetTexture(fontAtlasName));
    return material;
}

// std::shared_ptr<AbstractMaterial>
// CMaterialFactory::CreateUnlitMaterial(SDL_GPUTexture* texture) {

//     MaterialCreateInfo info;
//     info.type = EMaterialType::Unlit;
//     info.shaderPath = "assets/shaders/unlit";
//     info.texture = texture;
//     info.enableBlending = true;

//     return CreateMaterial(info);
// }

// std::shared_ptr<AbstractMaterial>
// CMaterialFactory::CreateTextMaterial(SDL_GPUTexture* fontAtlas) {

//     MaterialCreateInfo info;
//     info.type = EMaterialType::Text;
//     info.shaderPath = "assets/shaders/text";
//     info.texture = fontAtlas;
//     info.enableBlending = true;
//     info.blendMode = SDL_GPU_BLENDMODE_BLEND;

//     return CreateMaterial(info);
// }

} // namespace Material