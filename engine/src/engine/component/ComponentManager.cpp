#include "engine/component/ComponentManager.h"

// #include "engine/component/CollisionComponent.h"
#include "engine/component/CameraComponent.h"
#include "engine/component/InputComponent.h"
#include "engine/component/MovementComponent.h"
#include "engine/component/SpriteComponent.h"
#include "engine/component/TextComponent.h"
#include "engine/core/GameObject.h"

namespace Component {
CComponentManager::CComponentManager(Font::CFontHandler& fontHandler,
                                     Renderer::CTextRenderer& textRenderer)
    : mFontHandler(fontHandler), mTextRenderer(textRenderer) {
}

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

CTextComponent& CComponentManager::addTextComponent(Core::CGameObject& owner) {
    return createComponent<CTextComponent>(owner, owner.getId(), mTextRenderer);
}

CTransformComponent&
CComponentManager::addTransformComponent(Core::CGameObject& owner) {
    return createComponent<CTransformComponent>(owner, owner.getId());
}

CCameraComponent&
CComponentManager::addCameraComponent(Core::CGameObject& owner) {
    return createComponent<CCameraComponent>(owner, owner.getId());
}

void CComponentManager::removeComponents(Core::GameObjectId id) {
    for (auto& [type, pool] : mComponentPools) {
        removeComponent<decltype(type)>(id);
    }
}

void CComponentManager::update(float deltaTime) {
    updateAll<CInputComponent, CMovementComponent, CCameraComponent,
              CSpriteComponent>(deltaTime);
}

} // namespace Component
