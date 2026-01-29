#pragma once
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "engine/component/IComponent.h"
#include "engine/core/MeshTypes.h"
#include "engine/material/MaterialTypes.h"

namespace Material {
class CMaterialManager;
} // namespace Material

namespace Core {
class CMesh;
}

namespace Component {
class CMeshComponent : public IComponent {
public:
    explicit CMeshComponent(Core::GameObjectId owner,
                            CComponentManager& componentManager,
                            Material::CMaterialManager& materialManager);
    CMeshComponent(Core::GameObjectId owner,
                   CComponentManager& componentManager,
                   const CMeshComponent& other);
    CMeshComponent& operator=(const CMeshComponent& other);
    CMeshComponent(const CMeshComponent& other) = delete;
    CMeshComponent(const CMeshComponent&& other) = delete;
    CMeshComponent& operator=(CMeshComponent&& other) = delete;
    ~CMeshComponent() override;

    void SetTextureIndex(int index);
    void SetColor(const glm::vec4& color);

    int GetTextureIndex() const;
    glm::vec4 GetColor() const;

    void SetMesh(Core::CMesh* mesh);
    Core::CMesh* GetMesh() const;

    glm::vec2 GetSize();

    void SetMaterialType(Material::EMaterialType type);

private:
    Material::CMaterialManager& mMaterialManager;
    Core::CMesh* mMesh{nullptr};
    glm::vec4 mColor{1.0f, 0.0f, 1.0f, 1.0f};
    glm::vec2 mSize{0.0f, 0.0f};
    int mTextureIndex{0};
};
} // namespace Component
