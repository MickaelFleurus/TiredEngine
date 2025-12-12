#include "engine/core/GameObjectBuilder.h"

#include "engine/component/CameraComponent.h"
#include "engine/component/CollisionComponent.h"
#include "engine/component/ComponentManager.h"
#include "engine/component/InputComponent.h"
#include "engine/component/MeshComponent.h"
#include "engine/component/MovementComponent.h"
#include "engine/component/SpriteComponent.h"
#include "engine/component/TextUIComponent.h"
#include "engine/component/TransformComponent.h"
#include "engine/core/MeshManager.h"
#include "engine/font/FontHandler.h"

namespace Core {

GameObjectId CGameObjectBuilder::mNextId = 0;

CGameObjectBuilder::CGameObjectBuilder(
    Component::CComponentManager& componentManager,
    Font::CFontHandler& fontHandler, CMeshManager& meshManager)
    : mComponentManager(componentManager)
    , mFontHandler(fontHandler)
    , mMeshManager(meshManager) {
}

// Pass meshRenderer to builder
CGameObjectBuilder::CBuilder
CGameObjectBuilder::CreateBuilder(const std::string& name,
                                  CGameObject& parent) {
    return CBuilder(name, mComponentManager, mFontHandler, mMeshManager,
                    parent);
}

CGameObject*
CGameObjectBuilder::CreateRoot(Component::CComponentManager& componentManager) {
    return new CGameObject("Root", componentManager, nullptr, mNextId++);
}

// CGameObjectBuilder::CBuilder implementation

CGameObjectBuilder::CBuilder::CBuilder(
    const std::string& name, Component::CComponentManager& componentManager,
    Font::CFontHandler& fontHandler, CMeshManager& meshFactory,
    CGameObject& parent)
    : mComponentManager(componentManager)
    , mFontHandler(fontHandler)
    , mMeshManager(meshFactory)
    , mParent(parent)
    , mGameObject(std::unique_ptr<CGameObject>(new CGameObject(
          name, componentManager, &parent, CGameObjectBuilder::mNextId++))) {
}

CGameObjectBuilder::CBuilder&
CGameObjectBuilder::CBuilder::AddText(const std::string& text,
                                      unsigned int size, std::string fontName) {
    auto& component{mComponentManager.AddTextComponent(*mGameObject)};

    component.SetText(text);
    component.SetFontSize(size);
    component.SetPolice(&mFontHandler.GetPolice(fontName));
    return *this;
}

CGameObjectBuilder::CBuilder& CGameObjectBuilder::CBuilder::AddSprite() {
    auto& component{mComponentManager.AddSpriteComponent(*mGameObject)};
    // component.AddSprite(type);
    return *this;
}

CGameObjectBuilder::CBuilder&
CGameObjectBuilder::CBuilder::AddCameraComponent() {
    auto& component{mComponentManager.AddCameraComponent(*mGameObject)};
    // component.AddSprite(type);
    return *this;
}

// CGameObjectBuilder::CBuilder&
// CGameObjectBuilder::CBuilder::AddAABBCollisionData(float width, float height,
//                                                    bool isStatic,
//                                                    bool isTrigger) {

//     mComponentManager.addCollisionComponent(
//         *mGameObject, isStatic, Physics::SAABBParameters{width, height},
//         isTrigger);
//     return *this;
// }

// CGameObjectBuilder::CBuilder&
// CGameObjectBuilder::CBuilder::AddCircleCollisionData(float radius,
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
//         mComponentManager.GetComponent<Component::CCollisionComponent>(
//             mGameObject->GetId());
//     if (collisionComponent) {
//         collisionComponent->setCollisionCallback(std::move(callback));
//     }
//     return *this;
// }

CGameObjectBuilder::CBuilder&
CGameObjectBuilder::CBuilder::AddMovementData(float acceleration) {
    mComponentManager.AddMovementComponent(*mGameObject, acceleration);
    return *this;
}

CGameObjectBuilder::CBuilder&
CGameObjectBuilder::CBuilder::AddInputInfo(CInputCallbacks callbacks) {
    mComponentManager.AddInputComponent(*mGameObject, callbacks.onSpacePressed,
                                        callbacks.onLeftPressed,
                                        callbacks.onRightPressed);
    return *this;
}

CGameObjectBuilder::CBuilder&
CGameObjectBuilder::CBuilder::SetLocalPosition(const glm::vec3& position) {
    mComponentManager
        .GetComponent<Component::CTransformComponent>(mGameObject->GetId())
        ->SetPosition(position);
    return *this;
}

CGameObjectBuilder::CBuilder&
CGameObjectBuilder::CBuilder::SetAnchor(Utils::EAnchors anchor) {
    mComponentManager
        .GetComponent<Component::CTransformComponent>(mGameObject->GetId())
        ->SetAnchor(anchor);
    return *this;
}

CGameObjectBuilder::CBuilder&
CGameObjectBuilder::CBuilder::Add3DCube(float size) {
    auto* cube = mMeshManager.CreateCube(size);
    auto& meshComponent = mComponentManager.AddMeshComponent(*mGameObject);
    meshComponent.SetMesh(cube);

    return *this;
}

CGameObjectBuilder::CBuilder&
CGameObjectBuilder::CBuilder::Add3DQuad(float width, float height,
                                        float depth) {
    // Create mesh
    // Add mesh component
    return *this;
}

CGameObjectBuilder::CBuilder&
CGameObjectBuilder::CBuilder::SetMaterialType(Material::EMaterialType type) {
    auto* meshComponent =
        mComponentManager.GetComponent<Component::CMeshComponent>(
            mGameObject->GetId());
    if (meshComponent) {
        meshComponent->SetMaterialType(type);
    }
    return *this;
}

CGameObject* CGameObjectBuilder::CBuilder::Build() {
    auto* rawPtr = mGameObject.get();
    mParent.AddChild(std::move(mGameObject));

    return rawPtr;
}

} // namespace Core