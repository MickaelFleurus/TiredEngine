#include "engine/component/MeshComponent.h"

#include "engine/core/Mesh.h"
#include "engine/material/MaterialManager.h"

namespace Component {
CMeshComponent::CMeshComponent(Core::CGameObject& owner,
                               CComponentManager& componentManager,
                               Material::CMaterialManager& materialManager)
    : IComponent(owner, componentManager, Core::EDirtyType::None)
    , mMaterialManager(materialManager) {
}

CMeshComponent::~CMeshComponent() = default;

void CMeshComponent::SetMesh(Core::CMesh* mesh) {
    mMesh = mesh;

    SetDirty(true);
}

Core::CMesh* CMeshComponent::GetMesh() const {
    return mMesh;
}

void CMeshComponent::SetTextureIndex(int index) {
    mTextureIndex = index;
    AddDirtyFlag(Core::EDirtyType::InstanceProperties);
}

void CMeshComponent::SetColor(const glm::vec4& color) {
    mColor = color;
    AddDirtyFlag(Core::EDirtyType::InstanceProperties);
}

int CMeshComponent::GetTextureIndex() const {
    return mTextureIndex;
}

glm::vec4 CMeshComponent::GetColor() const {
    return mColor;
}

glm::vec2 CMeshComponent::GetSize() {
    if (mSize != glm::vec2(0.0f)) {
        return mSize;
    }

    if (mMesh == nullptr) {
        return glm::vec2(0.0f);
    }

    // Simple bounding box calculation
    const auto& vertices = mMesh->GetVertices();
    if (vertices.empty()) {
        return glm::vec2(0.0f);
    }

    float minX = vertices[0].position.x;
    float maxX = vertices[0].position.x;
    float minY = vertices[0].position.y;
    float maxY = vertices[0].position.y;

    for (const auto& vertex : vertices) {
        if (vertex.position.x < minX)
            minX = vertex.position.x;
        if (vertex.position.x > maxX)
            maxX = vertex.position.x;
        if (vertex.position.y < minY)
            minY = vertex.position.y;
        if (vertex.position.y > maxY)
            maxY = vertex.position.y;
    }
    mSize = glm::vec2(maxX - minX, maxY - minY);
    return mSize;
}

void CMeshComponent::SetMaterialType(Material::EMaterialType type) {
    if (mMesh == nullptr) {
        return;
    }

    auto* material = mMaterialManager.GetorCreateMaterial(type);
    mMesh->SetMaterial(material);
    AddDirtyFlag(Core::EDirtyType::Pipeline);
}

} // namespace Component
