#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <typeindex>
#include <unordered_map>

#include "component/IComponent.h"
#include "core/GameObjectId.h"
#include "input/InputCallback.h"
// #include "physics/CollisionType.h"

namespace SDK {
class Window;
}

namespace Core {
class CGameObject;
} // namespace Core

namespace Component {

class CSpriteComponent;
class CInputComponent;
// class CCollisionComponent;
class CMovementComponent;

class CComponentManager {
public:
    explicit CComponentManager();

    template <typename T>
    T* getComponent(int entityId) {
        auto& pool = mComponents[typeid(T)];
        auto it = pool.find(entityId);
        if (it != pool.end())
            return static_cast<T*>(it->second.get());
        return nullptr;
    }

    template <typename T>
    bool hasComponent(int entityId) {
        auto& pool = mComponents[typeid(T)];
        return pool.contains(entityId);
    }

    template <typename T>
    void removeComponent(Core::GameObjectId id) {
        auto& pool = mComponents[typeid(T)];
        pool.erase(id);
    }

    void removeComponents(Core::GameObjectId id);

    CInputComponent&
    addInputComponent(Core::CGameObject& owner,
                      std::optional<Input::InputFunc> onFirePressed,
                      std::optional<Input::InputFunc> onLeftPressed,
                      std::optional<Input::InputFunc> onRightPressed);

    CMovementComponent& addMovementComponent(Core::CGameObject& owner,
                                             float acceleration);

    // CCollisionComponent& addCollisionComponent(Core::CGameObject& owner, bool
    // isStatic,
    //                                            Physics::SCollisionParamVariant
    //                                            shape, bool isTrigger =
    //                                            false);

    CSpriteComponent& addSpriteComponent(Core::CGameObject& owner);

    void update(float deltaTime);

private:
    template <typename T, typename... Args>
    T& createComponent(Core::CGameObject& owner, Core::GameObjectId id,
                       Args&&... args) {
        auto component =
            std::make_unique<T>(owner, *this, std::forward<Args>(args)...);
        T* rawPtr = component.get(); // get raw pointer before moving
        mComponents[typeid(T)].emplace(id, std::move(component));
        return *rawPtr;
    }

    template <typename... Args>
    void updateAll(float deltaTime) {
        (update<Args>(deltaTime), ...);
    }

    template <typename T>
    void update(float deltaTime) {
        auto& pool = mComponents[typeid(T)];
        for (auto& componentPair : pool) {
            componentPair.second->update(deltaTime);
        }
    }

    std::unordered_map<std::type_index,
                       std::unordered_map<int, std::unique_ptr<IComponent>>>
        mComponents;
};
} // namespace Component