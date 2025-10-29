#include "engine/core/GameObjectBuilder.h"

#include "engine/component/CollisionComponent.h"
#include "engine/component/ComponentManager.h"
#include "engine/component/InputComponent.h"
#include "engine/component/MovementComponent.h"
#include "engine/component/SpriteComponent.h"

namespace Core {

GameObjectId CGameObjectBuilder::mNextId = 0;

CGameObjectBuilder::CGameObjectBuilder(
    Component::CComponentManager& componentManager)
    : mComponentManager(componentManager) {
}

CGameObjectBuilder::CBuilder
CGameObjectBuilder::CreateBuilder(const std::string& name,
                                  CGameObject& parent) {
    return CBuilder(name, mComponentManager, parent);
}

CGameObject*
CGameObjectBuilder::createRoot(Component::CComponentManager& componentManager) {
    return new CGameObject("Root", componentManager, nullptr, mNextId++);
}

// CGameObjectBuilder::CBuilder implementation

CGameObjectBuilder::CBuilder::CBuilder(
    const std::string& name, Component::CComponentManager& componentManager,
    CGameObject& parent)
    : mComponentManager(componentManager)
    , mParent(parent)
    , mGameObject(std::unique_ptr<CGameObject>(new CGameObject(
          name, componentManager, &parent, CGameObjectBuilder::mNextId++))) {
}

CGameObjectBuilder::CBuilder&
CGameObjectBuilder::CBuilder::addText(const std::string& text, unsigned size) {
    auto& component{mComponentManager.addSpriteComponent(*mGameObject)};
    component.addText(text, size);
    return *this;
}

CGameObjectBuilder::CBuilder& CGameObjectBuilder::CBuilder::addSprite() {
    auto& component{mComponentManager.addSpriteComponent(*mGameObject)};
    // component.addSprite(type);
    return *this;
}

// CGameObjectBuilder::CBuilder&
// CGameObjectBuilder::CBuilder::addAABBCollisionData(float width, float height,
//                                                    bool isStatic,
//                                                    bool isTrigger) {

//     mComponentManager.addCollisionComponent(
//         *mGameObject, isStatic, Physics::SAABBParameters{width, height},
//         isTrigger);
//     return *this;
// }

// CGameObjectBuilder::CBuilder&
// CGameObjectBuilder::CBuilder::addCircleCollisionData(float radius,
//                                                      bool isStatic,
//                                                      bool isTrigger) {
//     mComponentManager.addCollisionComponent(
//         *mGameObject, isStatic, Physics::SCircleParameters{radius},
//         isTrigger);
//     return *this;
// }

// CGameObjectBuilder::CBuilder&
// CGameObjectBuilder::CBuilder::addCollisionCallback(
//     std::function<void(Physics::SCollisionInfo)> callback) {
//     auto* collisionComponent =
//         mComponentManager.getComponent<Component::CCollisionComponent>(
//             mGameObject->getId());
//     if (collisionComponent) {
//         collisionComponent->setCollisionCallback(std::move(callback));
//     }
//     return *this;
// }

CGameObjectBuilder::CBuilder&
CGameObjectBuilder::CBuilder::addMovementData(float acceleration) {
    mComponentManager.addMovementComponent(*mGameObject, acceleration);
    return *this;
}

CGameObjectBuilder::CBuilder&
CGameObjectBuilder::CBuilder::addInputInfo(CInputCallbacks callbacks) {
    mComponentManager.addInputComponent(*mGameObject, callbacks.onSpacePressed,
                                        callbacks.onLeftPressed,
                                        callbacks.onRightPressed);
    return *this;
}

CGameObjectBuilder::CBuilder&
CGameObjectBuilder::CBuilder::setLocalPosition(const glm::vec3& position) {
    mComponentManager
        .getComponent<Component::CTransformComponent>(mGameObject->getId())
        ->setPosition(position);
    if (auto* spriteComponent =
            mComponentManager.getComponent<Component::CSpriteComponent>(
                mGameObject->getId());
        spriteComponent) {
        spriteComponent->updatePosition();
    }
    return *this;
}

CGameObject* CGameObjectBuilder::CBuilder::build() {
    auto* rawPtr = mGameObject.get();
    mParent.addChild(std::move(mGameObject));

    return rawPtr;
}

} // namespace Core