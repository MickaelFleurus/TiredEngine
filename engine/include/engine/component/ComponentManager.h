#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <typeindex>
#include <unordered_map>

#include "engine/component/IComponent.h"
#include "engine/component/TextComponent.h"
#include "engine/component/TransformComponent.h"
#include "engine/core/GameObjectId.h"
#include "engine/input/InputCallback.h"
// #include "engine/physics/CollisionType.h"

namespace SDK {
class Window;
}

namespace Core {
class CGameObject;
} // namespace Core

namespace Font {
class CFontHandler;
} // namespace Font

namespace Component {

class CSpriteComponent;
class CInputComponent;
// class CCollisionComponent;
class CMovementComponent;

class CComponentManager {
public:
    CComponentManager(Font::CFontHandler& fontHandler);

    template <typename T>
    T* getComponent(int entityId) {
        auto& pool = getComponentPool<T>();
        auto it = pool.mEntityToIndex.find(entityId);
        if (it != pool.mEntityToIndex.end()) {
            return static_cast<T*>(pool.mComponents[it->second].get());
        }
        return nullptr;
    }

    template <typename T>
    bool hasComponent(int entityId) {
        auto& pool = getComponentPool<T>();
        return pool.mEntityToIndex.contains(entityId);
    }

    template <typename T>
    void removeComponent(Core::GameObjectId id) {
        auto& pool = getComponentPool<T>();
        auto it = pool.mEntityToIndex.find(id);
        if (it == pool.mEntityToIndex.end()) {
            return;
        }

        const size_t indexToRemove = it->second;
        const size_t lastIndex = pool.mComponents.size() - 1;

        // Swap with last element if not already the last
        if (indexToRemove != lastIndex) {
            pool.mComponents[indexToRemove] =
                std::move(pool.mComponents[lastIndex]);
            pool.mEntityIds[indexToRemove] = pool.mEntityIds[lastIndex];

            // Update the moved component's index in the map
            pool.mEntityToIndex[pool.mEntityIds[indexToRemove]] = indexToRemove;
        }

        pool.mComponents.pop_back();
        pool.mEntityIds.pop_back();
        pool.mEntityToIndex.erase(id);
    }

    void removeComponents(Core::GameObjectId id);

    CInputComponent&
    addInputComponent(Core::CGameObject& owner,
                      std::optional<Input::InputFunc> onFirePressed,
                      std::optional<Input::InputFunc> onLeftPressed,
                      std::optional<Input::InputFunc> onRightPressed);

    CMovementComponent& addMovementComponent(Core::CGameObject& owner,
                                             float acceleration);

    CTextComponent& addTextComponent(Core::CGameObject& owner);

    // CCollisionComponent& addCollisionComponent(Core::CGameObject& owner, bool
    // isStatic,
    //                                            Physics::SCollisionParamVariant
    //                                            shape, bool isTrigger =
    //                                            false);

    CSpriteComponent& addSpriteComponent(Core::CGameObject& owner);

    CTransformComponent& addTransformComponent(Core::CGameObject& owner);

    void update(float deltaTime);

private:
    struct ComponentPool {
        std::vector<std::unique_ptr<IComponent>> mComponents;
        std::vector<Core::GameObjectId> mEntityIds;
        std::unordered_map<Core::GameObjectId, size_t> mEntityToIndex;
    };

    template <typename T>
    ComponentPool& getComponentPool() {
        return mComponentPools[std::type_index(typeid(T))];
    }

    template <typename T, typename... Args>
    T& createComponent(Core::CGameObject& owner, Core::GameObjectId id,
                       Args&&... args) {
        auto& pool = getComponentPool<T>();
        auto component =
            std::make_unique<T>(owner, *this, std::forward<Args>(args)...);
        T* rawPtr = component.get(); // get raw pointer before moving

        size_t newIndex = pool.mComponents.size();
        pool.mComponents.emplace_back(std::move(component));
        pool.mEntityIds.emplace_back(id);
        pool.mEntityToIndex[id] = newIndex;
        return *rawPtr;
    }

    template <typename... Args>
    void updateAll(float deltaTime) {
        (update<Args>(deltaTime), ...);
    }

    template <typename T>
    void update(float deltaTime) {
        auto& pool = mComponentPools[typeid(T)];
        for (auto& component : pool.mComponents) {
            component->update(deltaTime);
        }
    }

    Font::CFontHandler& mFontHandler;
    std::unordered_map<std::type_index, ComponentPool> mComponentPools;
};
} // namespace Component