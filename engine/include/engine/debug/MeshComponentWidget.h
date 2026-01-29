#pragma once

#include <string>
#include <vector>

#include <glm/vec4.hpp>

#include "engine/debug/IOverlordItem.h"

namespace Component {
class CMeshComponent;
}

namespace Core {
class CMeshManager;
}

namespace Debug {
class CTextureImGuiContainer;
class CMeshComponentWidget : public IOverlordItem {
public:
    explicit CMeshComponentWidget(Component::CMeshComponent& meshComponent,
                                  Core::CMeshManager& meshManager,
                                  CTextureImGuiContainer& textureContainer);
    void Render() override;
    const char* GetName() const override;

private:
    Component::CMeshComponent& mMeshComponent;
    Core::CMeshManager& mMeshManager;
    CTextureImGuiContainer& mTextureContainer;

    glm::vec4 mColor;
    std::string mCurrentMeshName{"Cube"};
    std::string mCurrentTextureName;
    int mCurrentTextureId{-1};
};
} // namespace Debug
