#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "engine/component/IComponent.h"
#include "engine/utils/Anchors.h"

namespace Component {
class CTransformComponent : public IComponent {
public:
    CTransformComponent(Core::CGameObject& owner,
                        CComponentManager& componentManager);

    void Update(float deltaTime) override;

    void SetPosition(const glm::vec3& position);
    const glm::vec3& GetPosition() const;

    void SetRotation(const glm::vec3& rotation);
    glm::vec3 GetRotation() const;

    void SetScale(const glm::vec3& scale);
    const glm::vec3& GetScale() const;

    void SetAnchor(Utils::EAnchors anchor);
    Utils::EAnchors GetAnchor() const;

    void UpdateMatrix(glm::mat4& parentTransform,
                      const glm::vec2& size = glm::vec2{1.0f}) const;

private:
    glm::vec3 mPosition{0.f, 0.f, 0.f};
    glm::vec3 mRotation{0.f, 0.f, 0.f};
    glm::quat mRotationQuat{1.f, 0.f, 0.f, 0.f};
    glm::vec3 mScale{1.f, 1.f, 1.f};
    Utils::EAnchors mAnchor{Utils::EAnchors::BottomLeft};
};
} // namespace Component