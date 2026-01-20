#pragma once
#include <optional>
#include <string>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

#include "engine/core/GameObjectId.h"

namespace Core {

class CCamera {
public:
    explicit CCamera(std::string cameraName = "Camera");

    const glm::mat4& GetViewMatrix();
    const glm::mat4& GetProjectionMatrix();
    const glm::mat4& GetViewProjection();

    glm::vec2 ScreenToWorld(const glm::vec2& screenPos,
                            const glm::vec2& viewportSize);
    glm::vec2 WorldToScreen(const glm::vec2& worldPos,
                            const glm::vec2& viewportSize);

    void SetAttachedGameObject(std::optional<Core::GameObjectId> gameobjectId);

    void SetZoom(float z);
    float GetZoom() const;
    void ZoomBy(float factor);

    void SetAspectRatio(float aspect);
    float GetAspectRatio() const;

    void SetClipPlanes(float n, float f);
    const glm::vec2& GetClipPlanes() const;

protected:
    virtual void EnsureUpToDate() = 0;

    std::optional<Core::GameObjectId> mGameObjectId;

    std::string mName;

    glm::mat4 mViewMatrix{1.0f};
    glm::mat4 mProjMatrix{1.0f};
    glm::mat4 mViewProjMatrix{1.0f};

    float mZoom{1.0f};
    float mAspectRatio{16.0f / 9.0f};
    glm::vec2 mNearFarZ{0.1f, 1000.0f};

    bool mIsDirty{true};
};
} // namespace Core
