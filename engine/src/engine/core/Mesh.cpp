#include "engine/core/Mesh.h"

namespace Core {
CMesh::CMesh(EMeshBaseType type, EMeshDynamicType dynamicType,
             std::span<Core::SVertex> vertices,
             const std::span<const uint32_t> indexes)
    : mBaseType(type)
    , mDynamicType(dynamicType)
    , mVertices(vertices.begin(), vertices.end())
    , mIndexes(indexes.begin(), indexes.end()) {
}

CMesh::~CMesh() = default;

const std::vector<SVertex>& CMesh::GetVertices() const {
    return mVertices;
}

const std::vector<uint32_t>& CMesh::GetIndexes() const {
    return mIndexes;
}

std::size_t CMesh::GetHash() const {
    return mHash;
}

void CMesh::SetHash(std::size_t hash) {
    mHash = hash;
}

Material::CAbstractMaterial* CMesh::GetMaterial() const {
    return mMaterial;
}

void CMesh::SetMaterial(Material::CAbstractMaterial* material) {
    mMaterial = material;
}

} // namespace Core