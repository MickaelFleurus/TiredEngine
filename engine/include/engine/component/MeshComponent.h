#pragma once
#include "engine/component/IDisplayComponent.h"

#include "engine/core/MeshTypes.h"

#include <glm/vec4.hpp>

namespace Core {
class CMesh;
}

namespace Renderer {
class CMeshRenderer;
} // namespace Renderer

namespace Component {
class CMeshComponent : public IDisplayComponent {
public:
    explicit CMeshComponent(Core::CGameObject& owner,
                            CComponentManager& componentManager);
    ~CMeshComponent() override;

    void SetTextureIndex(int index);
    void SetColor(const glm::vec4& color);

    int GetTextureIndex() const;
    glm::vec4 GetColor() const;

    void SetMesh(Core::CMesh* mesh);
    Core::CMesh* GetMesh() const;

    glm::vec2 GetSize() override;

private:
    Core::CMesh* mMesh{nullptr};
    glm::vec4 mColor{1.0f, 1.0f, 1.0f, 1.0f};
    int mTextureIndex{-1};
};
} // namespace Component
