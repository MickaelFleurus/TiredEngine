#pragma once
#include "engine/core/Camera.h"

namespace Core {
class CCamera3D : public CCamera {
public:
    explicit CCamera3D();

    void SetFOV(float fovDegrees);
    float GetFOV() const;

    void SetTarget(const glm::vec2& t);
    void ClearTarget();
    void SetSmoothingFactor(float factor);
    void SnapToTarget();

    void SetShakeOffset(const glm::vec3& shake);
    const glm::vec3& GetShakeOffset() const;

protected:
    void EnsureUpToDate() override;

private:
    float mFOV = 45.0f;
    float mZoom = 1.0f;
    float mAspectRatio = 16.0f / 9.0f;
    glm::vec2 mNearFarZ = {0.1f, 1000.0f};

    // TODO: Unused
    bool mSmoothingEnabled;
    glm::vec2 mTarget;
    float mSmoothingFactor;

    glm::vec3 mShakeOffset{0.0f};
};
} // namespace Core
