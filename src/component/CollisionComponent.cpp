// #include "component/CollisionComponent.h"
// #include "component/ComponentManager.h"
// #include "component/MovementComponent.h"
// #include "physics/CollisionDetection.h"

// #include "core/GameObject.h"

// namespace {
// template <typename A, typename B>
// std::optional<std::tuple<std::optional<glm::vec2>, glm::vec2>>
// collisionLogicImpl(const A& a, const B& b, Component::CMovementComponent&
// aMovementComponent) {
//     if (Physics::CheckCollision(a, b)) {
//         const auto velocity = aMovementComponent.getVelocity();
//         std::optional<glm::vec2> impact{std::nullopt};
//         auto penetrationVector = Physics::GetPenetrationVector(a, b, impact,
//         velocity); return std::make_tuple(impact, penetrationVector);
//     }
//     return std::nullopt;
// }

// Physics::SCollisionVariant CreateCollisionShape(Core::CGameObject& owner,
//                                                 Physics::SCollisionParamVariant
//                                                 param) {
//     return std::visit(
//         [&](auto&& arg) -> Physics::SCollisionVariant {
//             using T = std::decay_t<decltype(arg)>;
//             return Physics::getConcreteShape<T>(arg,
//             owner.getWorldPosition());
//         },
//         param);
// }

// glm::vec2 GetSafePosition(const glm::vec2& currentPosition,
//                                const glm::vec2& penetrationVector,
//                                const glm::vec2& direction) {
//     return currentPosition - penetrationVector * direction;
// }
// } // namespace

// namespace Component {
// std::vector<std::reference_wrapper<CCollisionComponent>>
// CCollisionComponent::kStaticColliders;
// std::vector<std::reference_wrapper<CCollisionComponent>>
// CCollisionComponent::kDynamicColliders;

// CCollisionComponent::CCollisionComponent(Core::CGameObject& owner,
//                                          CComponentManager& componentManager,
//                                          bool isStatic, bool isTrigger,
//                                          Physics::SCollisionParamVariant
//                                          param)
//     : IComponent(owner, componentManager)
//     , mIsStatic(isStatic)
//     , mIsTrigger(isTrigger)
//     , mCollisionData(param) {
//     if (mIsStatic) {
//         kStaticColliders.push_back(*this);
//     } else {
//         kDynamicColliders.push_back(*this);
//     }
// }

// CCollisionComponent::~CCollisionComponent() {
//     if (mIsStatic) {
//         auto it = std::find_if(kStaticColliders.begin(),
//         kStaticColliders.end(),
//                                [this](const CCollisionComponent& comp) {
//                                return &comp == this; });
//         if (it != kStaticColliders.end()) {
//             kStaticColliders.erase(it);
//         }
//     } else {
//         auto it = std::find_if(kDynamicColliders.begin(),
//         kDynamicColliders.end(),
//                                [this](const CCollisionComponent& comp) {
//                                return &comp == this; });
//         if (it != kDynamicColliders.end()) {
//             kDynamicColliders.erase(it);
//         }
//     }
// }

// void CCollisionComponent::setCollisionCallback(
//     std::function<void(Physics::SCollisionInfo)> callback) {
//     mCollisionCallback = std::move(callback);
// }

// void CCollisionComponent::update(float /*deltaTime*/) {
//     auto movementComponent =
//     mComponentManager.getComponent<CMovementComponent>(mOwner.getId()); if
//     (mIsStatic || !movementComponent) {
//         // Static colliders do not need to be updated
//         return;
//     }

//     const auto bb = CreateCollisionShape(mOwner, mCollisionData);
//     for (auto& colliderVector : {kStaticColliders, kDynamicColliders}) {
//         for (auto& collider : colliderVector) {
//             if (collider.get().mOwner.getId() == mOwner.getId()) {
//                 continue; // Skip self
//             }
//             collisionLogic(collider.get(), bb, *movementComponent);
//         }
//     }
// }

// void CCollisionComponent::collisionLogic(CCollisionComponent& other,
//                                          Physics::SCollisionVariant self,
//                                          CMovementComponent&
//                                          movementComponent) {

//     auto otherBb = CreateCollisionShape(other.mOwner, other.mCollisionData);
//     auto collisionData = std::visit(
//         [&](auto&& arg1, auto&& arg2) { return collisionLogicImpl(arg1, arg2,
//         movementComponent); }, self, otherBb);

//     if (!collisionData) {
//         return;
//     }

//     auto [impactOpt, penetrationVector] = *collisionData;

//     if (!penetrationVector.isZero() && !mIsTrigger && !other.mIsTrigger) {
//         movementComponent.applyPosition(GetSafePosition(
//             mOwner.getLocalPosition(), penetrationVector,
//             movementComponent.getDirection()));
//     }

//     Physics::SCollisionInfo collisionInfo{.own = CreateCollisionShape(mOwner,
//     mCollisionData),
//                                           .other = std::move(otherBb),
//                                           .penetrationVector =
//                                           penetrationVector, .impact =
//                                           impactOpt, .ownId = mOwner.getId(),
//                                           .otherId = other.mOwner.getId()};

//     if (mCollisionCallback) {
//         mCollisionCallback(collisionInfo);
//     }

//     if (other.mCollisionCallback) {
//         other.mCollisionCallback(collisionInfo);
//     }
// }
// } // namespace Component
