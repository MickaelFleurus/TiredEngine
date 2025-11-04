#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

#include "engine/component/IComponent.h"

namespace Component {
class CTransformComponent;
class CCameraComponent : public IComponent {
public:
    struct SMatrices {
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 model;
    };
    explicit CCameraComponent(Core::CGameObject& owner,
                              CComponentManager& componentManager);

    void SetZoom(float z);
    float GetZoom() const;
    void ZoomBy(float factor);
    void SetViewportSize(const glm::vec2& size);

    const glm::vec2& GetViewportSize() const;
    glm::vec4 GetScissorRect() const;

    void SetClipPlanes(float n, float f);
    const glm::vec2& GetClipPlanes() const;

    void SetWorldBounds(const glm::vec2& minBound, const glm::vec2& maxBound);

    void SetTarget(const glm::vec2& t);
    void ClearTarget();
    void SetSmoothingFactor(float factor);
    void SnapToTarget();

    void SetShakeOffset(const glm::vec3& shake);

    SMatrices GetMatrices();

    const glm::mat4& GetViewMatrix();
    const glm::mat4& GetProjectionMatrix();
    const glm::mat4& GetViewProjection();

    glm::vec2 ScreenToWorld(const glm::vec2& screenPos);

    glm::vec2 WorldToScreen(const glm::vec2& worldPos);
    glm::vec4 GetWorldAABB();

    void update(float dt) override;

    CTransformComponent& GetTransformComponent() const;

private:
    void EnsureUpToDate();

    CTransformComponent& mTransformComponent;
    float mZoom;
    glm::vec2 mViewportSize;
    glm::vec2 mNearFarZ;

    bool mSmoothingEnabled;
    glm::vec2 mTarget;
    float mSmoothingFactor;

    glm::vec2 mWorldMin;
    glm::vec2 mWorldMax;
    glm::vec3 mShakeOffset{0.0f};

    glm::mat4 mViewMatrix{1.0f};
    glm::mat4 mProjMatrix{1.0f};
    glm::mat4 mViewProjMatrix{1.0f};
};
} // namespace Component