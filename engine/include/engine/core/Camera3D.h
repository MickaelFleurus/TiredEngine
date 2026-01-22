#pragma once
#include "engine/core/Camera.h"
#include "engine/renderer/DirtyObserver.h"
#include "engine/utils/Token.h"

namespace Renderer {
class CTransformManager;
}

namespace Core {
class CCamera3D : public CCamera, public Renderer::IDirtyObserver {
public:
    explicit CCamera3D(Renderer::CTransformManager& transformManager);

    void SetFOV(float fovDegrees);
    float GetFOV() const;

    void SetTarget(const glm::vec2& t);
    void ClearTarget();
    void SetSmoothingFactor(float factor);
    void SnapToTarget();

    void SetShakeOffset(const glm::vec3& shake);
    const glm::vec3& GetShakeOffset() const;

    void OnDirty(Core::GameObjectId id) override;

protected:
    void EnsureUpToDate() override;

private:
    Renderer::CTransformManager& mTransformManager;
    float mFOV = 45.0f;
    float mZoom = 1.0f;
    float mAspectRatio = 16.0f / 9.0f;
    glm::vec2 mNearFarZ = {0.1f, 1000.0f};

    // TODO: Unused
    bool mSmoothingEnabled;
    glm::vec2 mTarget;
    float mSmoothingFactor;

    glm::vec3 mShakeOffset{0.0f};
    CToken mToken;
};
} // namespace Core
