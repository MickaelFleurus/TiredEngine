// #include "engine/core/GameObjectBuilder.h"

// #include "engine/component/CameraComponent.h"
// #include "engine/component/CollisionComponent.h"
// #include "engine/component/ComponentManager.h"
// #include "engine/component/InputComponent.h"
// #include "engine/component/MeshComponent.h"
// #include "engine/component/MovementComponent.h"
// #include "engine/component/SpriteComponent.h"
// #include "engine/component/TextUIComponent.h"
// #include "engine/core/GameObjectManager.h"
// #include "engine/core/MeshManager.h"
// #include "engine/font/FontHandler.h"
// #include "engine/renderer/TransformManager.h"
// #include "engine/utils/Logger.h"
// #include "engine/utils/StringId.h"

// namespace Core {

// CGameObjectBuilder::CGameObjectBuilder(
//     Component::CComponentManager& componentManager,
//     Font::CFontHandler& fontHandler, CMeshManager& meshFactory,
//     CGameObjectManager& gameObjectManager)
//     : mComponentManager(componentManager)
//     , mFontHandler(fontHandler)
//     , mMeshManager(meshFactory)
//     , mGameObjectManager(gameObjectManager) {
// }

// CGameObjectBuilder& CGameObjectBuilder::Start(const std::string& name) {
//     if (mCreatedObj) {
//         LOG_WARNING("A new gameobject creation was started while another one
//         "
//                     "was not finished yet.");
//         mGameObjectManager.Destroy(*mCreatedObj);
//     }
//     mCreatedObj = mGameObjectManager.CreateObject(name);

//     return *this;
// }

// CGameObjectBuilder& CGameObjectBuilder::AddText(const std::string& text,
//                                                 unsigned int size,
//                                                 std::string fontName) {
//     if (mCreatedObj) {
//         auto& component{mComponentManager.AddTextComponent(*mCreatedObj)};

//         component.SetText(text);
//         component.SetFontSize(size);
//         component.SetPolice(&mFontHandler.GetPolice(fontName));
//     }
//     return *this;
// }

// CGameObjectBuilder& CGameObjectBuilder::AddSprite(std::string spriteName) {
//     if (mCreatedObj) {
//         auto& component{mComponentManager.AddSpriteComponent(*mCreatedObj)};
//         component.SetSprite(spriteName);
//     }
//     return *this;
// }

// CGameObjectBuilder& CGameObjectBuilder::AddCameraComponent() {
//     if (mCreatedObj) {
//         mComponentManager.AddCameraComponent(*mCreatedObj);
//     }
//     return *this;
// }

// CGameObjectBuilder&
// CGameObjectBuilder::SetLocalPosition(const glm::vec3& position) {
//     if (mCreatedObj) {
//         mGameObjectManager.GetTransformManager().SetPosition(*mCreatedObj,
//                                                              position);
//     }
//     return *this;
// }

// CGameObjectBuilder& CGameObjectBuilder::SetAnchor(Utils::EAnchors anchor) {
//     if (mCreatedObj) {
//         // mComponentManager
//         //
//         .GetComponent<Component::CTransformComponent>(mGameObject->GetId())
//         //     ->SetAnchor(anchor);
//     }
//     return *this;
// }

// CGameObjectBuilder& CGameObjectBuilder::Add3DCube(float size) {
//     if (mCreatedObj) {
//         auto* cube = mMeshManager.CreateCube(size);
//         auto& meshComponent =
//         mComponentManager.AddMeshComponent(*mCreatedObj);
//         meshComponent.SetMesh(cube);
//     }

//     return *this;
// }

// CGameObjectBuilder& CGameObjectBuilder::Add3DQuad(float width, float height,
//                                                   float depth) {
//     // Create mesh
//     // Add mesh component
//     if (mCreatedObj) {
//     }
//     return *this;
// }

// CGameObjectBuilder&
// CGameObjectBuilder::SetMaterialType(Material::EMaterialType type) {
//     if (mCreatedObj) {
//         auto* meshComponent =
//             mComponentManager.GetComponent<Component::CMeshComponent>(
//                 *mCreatedObj);
//         if (meshComponent) {
//             meshComponent->SetMaterialType(type);
//         }
//     }
//     return *this;
// }

// Core::GameObjectId CGameObjectBuilder::Build() {
//     if (!mCreatedObj) {
//         LOG_ERROR("Trying to build a game object that was never started. Call
//         "
//                   "Start() before doing anything else.");
//     }
//     auto created = *mCreatedObj;
//     mCreatedObj = std::nullopt;
//     return created;
// }

// Core::GameObjectId CGameObjectBuilder::CloneObject(GameObjectId cloned,
//                                                    const std::string& name) {
//     CStringId objName{name};
//     if (name.empty()) {
//         objName = mGameObjectManager.GetStringId(cloned) + "_cloned";
//     }
//     auto clone = mGameObjectManager.CreateObject(objName);
//     mComponentManager.CloneComponents(clone, cloned);
//     mGameObjectManager.GetTransformManager().Clone(clone, cloned);
//     return clone;
// }

// } // namespace Core
