#include "engine/component/ComponentManager.h"

// #include "engine/component/CollisionComponent.h"
#include "engine/component/InputComponent.h"
#include "engine/component/MovementComponent.h"
#include "engine/component/SpriteComponent.h"
#include "engine/core/GameObject.h"

namespace Component {
CInputComponent& CComponentManager::addInputComponent(
    Core::CGameObject& owner, std::optional<Input::InputFunc> onFirePressed,
    std::optional<Input::InputFunc> onLeftPressed,
    std::optional<Input::InputFunc> onRightPressed) {
    return createComponent<CInputComponent>(owner, owner.getId(), onFirePressed,
                                            onLeftPressed, onRightPressed);
}

CMovementComponent&
CComponentManager::addMovementComponent(Core::CGameObject& owner,
                                        float acceleration) {
    return createComponent<CMovementComponent>(owner, owner.getId(),
                                               acceleration);
}

// CCollisionComponent&
// CComponentManager::addCollisionComponent(Core::CGameObject& owner,
//                                                               bool isStatic,
//                                                               Physics::SCollisionParamVariant
//                                                               shape, bool
//                                                               isTrigger) {
//     return createComponent<CCollisionComponent>(owner, owner.getId(),
//     isStatic, isTrigger, shape);
// }

CSpriteComponent&
CComponentManager::addSpriteComponent(Core::CGameObject& owner) {
    return createComponent<CSpriteComponent>(owner, owner.getId());
}

void CComponentManager::removeComponents(Core::GameObjectId id) {
    for (auto& [type, pool] : mComponentPools) {
        removeComponent<decltype(type)>(id);
    }
}

void CComponentManager::update(float deltaTime) {
    updateAll<CInputComponent, CMovementComponent, CSpriteComponent>(deltaTime);
}

} // namespace Component
