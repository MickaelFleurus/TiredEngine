#include "engine/component/ComponentManager.h"

// #include "engine/component/CollisionComponent.h"
#include "engine/component/CameraComponent.h"
#include "engine/component/InputComponent.h"
#include "engine/component/MeshComponent.h"
#include "engine/component/MovementComponent.h"
#include "engine/component/SpriteComponent.h"
#include "engine/component/TextComponent.h"
#include "engine/component/TransformComponent.h"
#include "engine/core/GameObject.h"

namespace Component {
CComponentManager::CComponentManager(
    Font::CFontHandler& fontHandler, Renderer::CTextRenderer& textRenderer,
    Material::CMaterialManager& materialManager)
    : mFontHandler(fontHandler)
    , mTextRenderer(textRenderer)
    , mMaterialManager(materialManager) {
}

CInputComponent& CComponentManager::AddInputComponent(
    Core::CGameObject& owner, std::optional<Input::InputFunc> onFirePressed,
    std::optional<Input::InputFunc> onLeftPressed,
    std::optional<Input::InputFunc> onRightPressed) {
    return CreateComponent<CInputComponent>(owner, owner.GetId(), onFirePressed,
                                            onLeftPressed, onRightPressed);
}

CMovementComponent&
CComponentManager::AddMovementComponent(Core::CGameObject& owner,
                                        float acceleration) {
    return CreateComponent<CMovementComponent>(owner, owner.GetId(),
                                               acceleration);
}

// CCollisionComponent&
// CComponentManager::addCollisionComponent(Core::CGameObject& owner,
//                                                               bool isStatic,
//                                                               Physics::SCollisionParamVariant
//                                                               shape, bool
//                                                               isTrigger) {
//     return CreateComponent<CCollisionComponent>(owner, owner.GetId(),
//     isStatic, isTrigger, shape);
// }

CSpriteComponent&
CComponentManager::AddSpriteComponent(Core::CGameObject& owner) {
    return CreateComponent<CSpriteComponent>(owner, owner.GetId());
}

CTextComponent& CComponentManager::AddTextComponent(Core::CGameObject& owner) {
    return CreateComponent<CTextComponent>(owner, owner.GetId());
}

CTransformComponent&
CComponentManager::AddTransformComponent(Core::CGameObject& owner) {
    return CreateComponent<CTransformComponent>(owner, owner.GetId());
}

CCameraComponent&
CComponentManager::AddCameraComponent(Core::CGameObject& owner) {
    return CreateComponent<CCameraComponent>(owner, owner.GetId());
}

CMeshComponent& CComponentManager::AddMeshComponent(Core::CGameObject& owner) {
    return CreateComponent<CMeshComponent>(owner, owner.GetId(),
                                           mMaterialManager);
}

void CComponentManager::RemoveComponents(Core::GameObjectId id) {
    for (auto& [type, pool] : mComponentPools) {
        RemoveComponent<decltype(type)>(id);
    }
}

void CComponentManager::Update(float deltaTime) {
    UpdateAll<CInputComponent, CMovementComponent, CCameraComponent,
              CSpriteComponent>(deltaTime);
}

} // namespace Component
