#include "engine/component/CameraComponent.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>

#include "engine/component/ComponentManager.h"
#include "engine/core/CameraManager.h"
#include "engine/core/GameObject.h"

namespace Component {
CCamera3DComponent::CCamera3DComponent(Core::GameObjectId owner,
                                       CComponentManager& componentManager,
                                       Core::CCameraManager& cameraManager)
    : IComponent(owner, componentManager), mCameraManager(cameraManager) {
    mCameraManager.GetDefaultCamera3D().SetAttachedGameObject(owner);
}
CCamera3DComponent::~CCamera3DComponent() {
    mCameraManager.GetDefaultCamera3D().SetAttachedGameObject(std::nullopt);
}

void CCamera3DComponent::Update(float dt) {
}
} // namespace Component
