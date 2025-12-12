#include "engine/material/MaterialFactory.h"

#include "engine/material/AbstractMaterial.h"
#include "engine/material/Material.h"
#include "engine/renderer/PipelineTypes.h"
#include "engine/renderer/TextureManager.h"
#include "engine/utils/FileHandler.h"

namespace Material {

CMaterialFactory::CMaterialFactory(
    Renderer::CTextureManager& textureManager, Utils::CFileHandler& fileHandler,
    const Vulkan::CVulkanContext& contextGetter,
    Vulkan::CDescriptorStorage& descriptorStorage)
    : mTextureManager(textureManager)
    , mFileHandler(fileHandler)
    , mPipelineFactory(contextGetter)
    , mDescriptorStorage(descriptorStorage) {
}

CMaterialFactory::~CMaterialFactory() = default;

std::unique_ptr<CAbstractMaterial>
CMaterialFactory::CreateMaterial(EMaterialType type,
                                 const Renderer::SPipelineConfig& info) {

    auto pipeline =
        mPipelineFactory.GetOrCreateGraphicsPipeline(info, mDescriptorStorage);
    return std::make_unique<CMaterial>(type, info.vertexLayout, pipeline);
}

std::unique_ptr<CAbstractMaterial>
CMaterialFactory::GetMaterial(EMaterialType type) {
    switch (type) {
    case EMaterialType::Normal:
        return CreateMaterial(
            type, Renderer::SPipelineConfig{
                      .shaderName = "NormalShader",
                      .shaderPath = mFileHandler.GetAssetsFolder() + "shaders/",
                      .vertexLayout = Renderer::EVertexLayout::Mesh3D});
    case EMaterialType::UI:
        return CreateUIMaterial();
    default:
        return nullptr;
    }
}

std::unique_ptr<CAbstractMaterial> CMaterialFactory::CreateUIMaterial() {
    Renderer::SPipelineConfig config;
    config.shaderName = "UIShader";
    config.shaderPath = mFileHandler.GetAssetsFolder() + "shaders/";
    config.vertexLayout = Renderer::EVertexLayout::UI;
    config.cullMode = Renderer::ECullMode::None;
    config.primitiveType = Renderer::EPrimitiveType::TriangleList;
    config.frontFace = Renderer::EFrontFace::Clockwise;
    config.enableBlending = true;
    config.enableDepthTest = false;

    auto material = CreateMaterial(EMaterialType::Text, config);
    return material;
}

} // namespace Material