#pragma once

#include "engine/component/IComponent.h"

namespace Core {
class CCameraManager;
}

namespace Component {
class CCamera3DComponent : public IComponent {
public:
    explicit CCamera3DComponent(Core::GameObjectId owner,
                                CComponentManager& componentManager,
                                Core::CCameraManager& cameraManager);
    ~CCamera3DComponent() override;

    void Update(float dt) override;

private:
    Core::CCameraManager& mCameraManager;
};
} // namespace Component
