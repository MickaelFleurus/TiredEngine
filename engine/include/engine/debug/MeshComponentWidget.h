#pragma once

#include "engine/debug/IOverlordItem.h"

#include <glm/vec4.hpp>
#include <string>
#include <vector>

namespace Component {
class CMeshComponent;
}

namespace Debug {
class CMeshComponentWidget : public IOverlordItem {
public:
    CMeshComponentWidget(Component::CMeshComponent& meshComponent);
    void Render() override;
    const char* GetName() const override;

private:
    Component::CMeshComponent& mMeshComponent;
    glm::vec4 mColor;
};
} // namespace Debug