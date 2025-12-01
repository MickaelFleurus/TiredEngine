#pragma once

#include "engine/core/DataTypes.h"
#include "engine/core/MeshTypes.h"
#include <span>

#include <vector>

namespace Material {
class CAbstractMaterial;
}

namespace Core {
class CMesh {
public:
    CMesh(EMeshBaseType type, EMeshDynamicType dynamicType,
          std::span<Core::SVertex> vertices,
          const std::span<const uint32_t> indexes);
    ~CMesh();

    const std::vector<SVertex>& GetVertices() const;
    const std::vector<uint32_t>& GetIndexes() const;

    std::size_t GetHash() const;
    void SetHash(std::size_t hash);

    Material::CAbstractMaterial* GetMaterial() const;
    void SetMaterial(Material::CAbstractMaterial* material);

private:
    const EMeshBaseType mBaseType;
    const EMeshDynamicType mDynamicType;
    const std::vector<Core::SVertex> mVertices;
    const std::vector<uint32_t> mIndexes;

    std::size_t mHash;
    Material::CAbstractMaterial* mMaterial{nullptr};
};
} // namespace Core