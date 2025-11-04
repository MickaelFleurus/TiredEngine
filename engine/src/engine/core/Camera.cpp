#include "engine/core/Camera.h"

#include "engine/component/CameraComponent.h"
#include "engine/component/ComponentManager.h"
#include "engine/core/GameObjectBuilder.h"

namespace {
constexpr const char* kCameraName = "Camera";

Core::CGameObject& BuildEntity(Core::CGameObjectBuilder::CBuilder builder,
                               Component::CComponentManager& componentManager) {
    Core::CGameObject& entity = *builder.addCameraComponent().build();
    return entity;
}
} // namespace

namespace Core {
CCamera::CCamera(CGameObject& parent, CGameObjectBuilder& builder,
                 Component::CComponentManager& componentManager)
    : mEntity(BuildEntity(builder.CreateBuilder(kCameraName, parent),
                          componentManager))
    , mCameraComponent(
          *componentManager.getComponent<Component::CCameraComponent>(
              mEntity.getId())) {
}

Component::CCameraComponent& CCamera::GetCameraComponent() {
    return mCameraComponent;
}

} // namespace Core