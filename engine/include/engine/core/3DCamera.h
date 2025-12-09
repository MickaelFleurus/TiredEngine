#pragma once
#include "engine/core/Camera.h"

namespace Core {
class C3DCamera : public CCamera {
public:
    C3DCamera(CGameObject& parent, CGameObjectBuilder& builder,
              Component::CComponentManager& componentManager);

    void SetFOV(float fovDegrees);
    float GetFOV() const;

protected:
    void EnsureUpToDate() override;

private:
    float mFOV = 45.0f; // Field of view in degrees
};
} // namespace Core