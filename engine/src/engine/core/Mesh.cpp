#include "engine/core/Mesh.h"

#include "engine/renderer/MeshRenderer.h"

namespace Core {
CMesh::CMesh(Renderer::CMeshRenderer& meshRenderer, std::size_t hash,
             EMeshBaseType type, EMeshDynamicType dynamicType,
             std::span<Core::SVertex> vertices,
             const std::span<const uint32_t> indexes)
    : mBaseType(type)
    , mDynamicType(dynamicType)
    , mVertices(vertices.begin(), vertices.end())
    , mIndexes(indexes.begin(), indexes.end())
    , mMeshRenderer(meshRenderer)
    , mHash(hash) {
    mMeshRenderer.RegisterMesh(this);
}
CMesh::CMesh(CMesh&& other) noexcept
    : mBaseType(other.mBaseType)
    , mDynamicType(other.mDynamicType)
    , mVertices(std::move(other.mVertices))
    , mIndexes(std::move(other.mIndexes))
    , mMeshRenderer(other.mMeshRenderer)
    , mHash(other.mHash)
    , mMaterial(other.mMaterial) {
}

CMesh& CMesh::operator=(CMesh&& other) noexcept {
    if (this != &other) {
        // Destructor will handle cleanup of old data
        mVertices = std::move(other.mVertices);
        mIndexes = std::move(other.mIndexes);
        mMaterial = other.mMaterial;
    }
    return *this;
}

CMesh::~CMesh() {
    if (mVertices.empty() || mIndexes.empty()) {
        return;
    }
    mMeshRenderer.UnregisterMesh(this);
}

const std::vector<SVertex>& CMesh::GetVertices() const {
    return mVertices;
}

const std::vector<Core::IndexType>& CMesh::GetIndexes() const {
    return mIndexes;
}

std::size_t CMesh::GetHash() const {
    return mHash;
}

Material::CAbstractMaterial* CMesh::GetMaterial() const {
    return mMaterial;
}

void CMesh::SetMaterial(Material::CAbstractMaterial* material) {
    mMaterial = material;
}

} // namespace Core