#pragma once

#include "engine/debug/IOverlordItem.h"
#include "engine/renderer/TransformHandle.h"

namespace Debug {

class CTransformWidget : public IOverlordItem {
public:
    CTransformWidget(Renderer::CTransformHandle transform);
    void Render() override;
    const char* GetName() const override;

private:
    Renderer::CTransformHandle mTransform;
    bool mIsInitialized{false};
    glm::vec3 mRotationAngles;
};
} // namespace Debug
