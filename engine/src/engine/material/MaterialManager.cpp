#include "engine/material/MaterialManager.h"

#include "engine/material/AbstractMaterial.h"
#include "engine/material/MaterialFactory.h"

namespace Material {

CMaterialManager::CMaterialManager(
    Renderer::CTextureManager& textureManager, Utils::CFileHandler& fileHandler,
    const Vulkan::CVulkanContext& contextGetter,
    Vulkan::CDescriptorStorage& descriptorStorage)
    : mMaterialFactory(textureManager, fileHandler, contextGetter,
                       descriptorStorage) {
}

CMaterialManager::~CMaterialManager() = default;

CAbstractMaterial* CMaterialManager::GetorCreateMaterial(EMaterialType type) {
    auto it = mMaterials.find(type);
    if (it != mMaterials.end()) {
        return it->second.get();
    }

    auto material = mMaterialFactory.GetMaterial(type);
    CAbstractMaterial* materialPtr = material.get();
    mMaterials.emplace(type, std::move(material));
    return materialPtr;
}

CAbstractMaterial*
CMaterialManager::GetMaterialById(std::size_t materialId) const {
    for (const auto& [type, material] : mMaterials) {
        if (material->GetId() == materialId) {
            return material.get();
        }
    }
    return nullptr;
}
} // namespace Material