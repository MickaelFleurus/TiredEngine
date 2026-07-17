#pragma once
#include <unordered_map>

#include "engine/material/MaterialFactory.h"
#include "engine/material/MaterialTypes.h"

namespace Vulkan {
struct SContext;
class CSwapchain;
class CDescriptorStorage;
} // namespace Vulkan

namespace Renderer {
class CTextureManager;
} // namespace Renderer

namespace Utils {
class CFileHandler;
}

namespace Material {
class CAbstractMaterial;

class CMaterialManager {
public:
    explicit CMaterialManager(Renderer::CTextureManager& textureManager,
                              Utils::CFileHandler& fileHandler,
                              const Vulkan::SContext& context,
                              Vulkan::CSwapchain& swapchain,
                              Vulkan::CDescriptorStorage& descriptorStorage);
    ~CMaterialManager();

    CAbstractMaterial* GetorCreateMaterial(EMaterialType type);
    CAbstractMaterial* GetMaterialById(std::size_t materialId) const;

private:
    CMaterialFactory mMaterialFactory;
    std::unordered_map<EMaterialType, std::unique_ptr<CAbstractMaterial>>
        mMaterials;
};
} // namespace Material
