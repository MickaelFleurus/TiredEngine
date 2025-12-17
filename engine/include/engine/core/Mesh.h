#pragma once

#include <span>
#include <vector>

#include "engine/core/DataTypes.h"
#include "engine/core/MeshTypes.h"

namespace Material {
class CAbstractMaterial;
}

namespace Renderer {
class CMeshRenderer;
}

namespace Core {

class CMesh {
public:
    CMesh(Renderer::CMeshRenderer& meshRenderer, std::size_t hash,
          EMeshBaseType type, EMeshDynamicType dynamicType,
          std::span<Core::SVertex> vertices,
          const std::span<const uint32_t> indexes);
    CMesh(CMesh&& other) noexcept;
    CMesh& operator=(CMesh&& other) noexcept;

    CMesh(CMesh const& other) = delete;
    CMesh& operator=(CMesh const& other) = delete;
    ~CMesh();

    const std::vector<SVertex>& GetVertices() const;
    const std::vector<IndexType>& GetIndexes() const;

    std::size_t GetHash() const;

    Material::CAbstractMaterial* GetMaterial() const;
    void SetMaterial(Material::CAbstractMaterial* material);

private:
    const EMeshBaseType mBaseType;
    EMeshDynamicType mDynamicType;
    std::vector<Core::SVertex> mVertices;
    std::vector<IndexType> mIndexes;

    Renderer::CMeshRenderer& mMeshRenderer;
    const std::size_t mHash;
    Material::CAbstractMaterial* mMaterial{nullptr};
};
} // namespace Core