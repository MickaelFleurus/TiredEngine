#include "engine/component/MeshComponent.h"
#include "engine/core/Mesh.h"

namespace Component {
CMeshComponent::CMeshComponent(Core::CGameObject& owner,
                               CComponentManager& componentManager)
    : IDisplayComponent(owner, componentManager) {
}

CMeshComponent::~CMeshComponent() = default;

void CMeshComponent::SetMesh(Core::CMesh* mesh) {
    mMesh = mesh;

    setDirty(true);
}

Core::CMesh* CMeshComponent::GetMesh() const {
    return mMesh;
}

void CMeshComponent::SetTextureIndex(int index) {
    mTextureIndex = index;
    setDirty(true);
}

void CMeshComponent::SetColor(const glm::vec4& color) {
    mColor = color;
    setDirty(true);
}

int CMeshComponent::GetTextureIndex() const {
    return mTextureIndex;
}

glm::vec4 CMeshComponent::GetColor() const {
    return mColor;
}

glm::vec2 CMeshComponent::GetSize() {
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

    return glm::vec2(maxX - minX, maxY - minY);
}

} // namespace Component