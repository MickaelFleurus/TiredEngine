#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

#include "engine/component/IComponent.h"

namespace Core {
class CCameraManager;
}

namespace Component {
class CTransformComponent;
class CCamera3DComponent : public IComponent {
public:
    explicit CCamera3DComponent(Core::CGameObject& owner,
                                CComponentManager& componentManager,
                                Core::CCameraManager& cameraManager);

    void Update(float dt) override;

private:
    Core::CCameraManager& mCameraManager;
};
} // namespace Component