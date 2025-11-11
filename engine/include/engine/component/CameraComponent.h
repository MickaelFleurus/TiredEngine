#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

#include "engine/component/IComponent.h"

namespace Component {
class CTransformComponent;
class CCameraComponent : public IComponent {
public:
    explicit CCameraComponent(Core::CGameObject& owner,
                              CComponentManager& componentManager);

    void SetZoom(float z);
    float GetZoom() const;
    void ZoomBy(float factor);

    void SetOrthographicSize(float size);
    float GetOrthographicSize() const;

    void SetAspectRatio(float aspect);
    float GetAspectRatio() const;

    void SetClipPlanes(float n, float f);
    const glm::vec2& GetClipPlanes() const;

    void SetWorldBounds(const glm::vec2& minBound, const glm::vec2& maxBound);

    void SetTarget(const glm::vec2& t);
    void ClearTarget();
    void SetSmoothingFactor(float factor);
    void SnapToTarget();

    void SetShakeOffset(const glm::vec3& shake);
    const glm::vec3& GetShakeOffset() const;

    void update(float dt) override;

private:
    float mZoom;
    float mAspectRatio;
    glm::vec2 mNearFarZ;
    float mOrthographicSize;

    bool mSmoothingEnabled;
    glm::vec2 mTarget;
    float mSmoothingFactor;

    glm::vec2 mWorldMin;
    glm::vec2 mWorldMax;
    glm::vec3 mShakeOffset{0.0f};
};
} // namespace Component