#pragma once
#include <glm/mat4x4.hpp>
#include <string>

namespace Component {
class CCameraComponent;
class CComponentManager;
class CTransformComponent;
} // namespace Component

namespace Core {
class CGameObjectBuilder;
class CGameObject;
class CCamera {
public:
    struct SMatrices {
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 model;
    };
    CCamera(CGameObject& parent, CGameObjectBuilder& builder,
            Component::CComponentManager& componentManager,
            std::string cameraName = "Camera");

    SMatrices GetMatrices();

    const glm::mat4& GetViewMatrix();
    const glm::mat4& GetProjectionMatrix();
    const glm::mat4& GetViewProjection();

    glm::vec2 ScreenToWorld(const glm::vec2& screenPos,
                            const glm::vec2& viewportSize);
    glm::vec2 WorldToScreen(const glm::vec2& worldPos,
                            const glm::vec2& viewportSize);

protected:
    virtual void EnsureUpToDate();

    CGameObject& mEntity;
    Component::CCameraComponent& mCameraComponent;
    Component::CTransformComponent& mTransformComponent;

    glm::mat4 mViewMatrix{1.0f};
    glm::mat4 mProjMatrix{1.0f};
    glm::mat4 mViewProjMatrix{1.0f};
};
} // namespace Core