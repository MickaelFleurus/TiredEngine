// #pragma once
// #include "component/IComponent.h"
// #include "physics/CollisionInfo.h"
// #include "physics/CollisionType.h"

// #include <variant>
// #include <vector>

// namespace Component {
// class CMovementComponent;
// class CCollisionComponent : public IComponent {
// public:
//     explicit CCollisionComponent(Core::CGameObject& owner, CComponentManager&
//     componentManager,
//                                  bool isStatic, bool isTrigger,
//                                  Physics::SCollisionParamVariant param);

//     ~CCollisionComponent() override;

//     void setCollisionCallback(std::function<void(Physics::SCollisionInfo)>
//     callback);

//     void update(float deltaTime) override;

// private:
//     static std::vector<std::reference_wrapper<CCollisionComponent>>
//     kStaticColliders; static
//     std::vector<std::reference_wrapper<CCollisionComponent>>
//     kDynamicColliders;

//     void collisionLogic(CCollisionComponent& other,
//     Physics::SCollisionVariant self,
//                         CMovementComponent& movementComponent);

//     bool mIsStatic{false};
//     bool mIsTrigger{false};
//     Physics::SCollisionParamVariant mCollisionData;

//     std::function<void(Physics::SCollisionInfo)> mCollisionCallback{nullptr};
// };
// } // namespace Component