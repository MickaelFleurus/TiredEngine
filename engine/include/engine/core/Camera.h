#pragma once

namespace Component {
class CCameraComponent;
class CComponentManager;
} // namespace Component

namespace Core {
class CGameObjectBuilder;
class CGameObject;
class CCamera {
public:
    CCamera(CGameObject& parent, CGameObjectBuilder& builder,
            Component::CComponentManager& componentManager);

    Component::CCameraComponent& GetCameraComponent();

private:
    CGameObject& mEntity;
    Component::CCameraComponent& mCameraComponent;

    bool mDirty;
};
} // namespace Core