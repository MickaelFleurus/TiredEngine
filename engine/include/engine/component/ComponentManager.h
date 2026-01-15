#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <typeindex>
#include <unordered_map>

#include "engine/component/ComponentType.h"
#include "engine/component/IComponent.h"
#include "engine/core/GameObjectId.h"
#include "engine/input/InputCallback.h"

namespace Core {
class CGameObject;
} // namespace Core

namespace Material {
class CMaterialManager;
} // namespace Material

namespace Font {
class CFontHandler;
} // namespace Font

namespace Renderer {
class CTextRenderer;
class CSpriteManager;
} // namespace Renderer

namespace Component {

class CSpriteComponent;
class CInputComponent;
class CCameraComponent;
class CTextUIComponent;
class CMovementComponent;
class CMeshComponent;
class CTransformComponent;

class CComponentManager {
public:
    CComponentManager(Font::CFontHandler& fontHandler,
                      Renderer::CTextRenderer& textRenderer,
                      Material::CMaterialManager& materialManager,
                      Renderer::CSpriteManager& spriteManager);

    template <typename T>
    T* GetComponent(int entityId) {
        return static_cast<T*>(
            GetComponent(entityId, GetComponentType(typeid(T))));
    }

    bool HasComponent(int entityId, EComponentType type) const;

    void RemoveComponent(Core::GameObjectId id, EComponentType type);
    void RemoveComponents(Core::GameObjectId id);

    CInputComponent&
    AddInputComponent(Core::CGameObject& owner,
                      std::optional<Input::InputFunc> onFirePressed,
                      std::optional<Input::InputFunc> onLeftPressed,
                      std::optional<Input::InputFunc> onRightPressed);
    CMovementComponent& AddMovementComponent(Core::CGameObject& owner,
                                             float acceleration);
    CTextUIComponent& AddTextComponent(Core::CGameObject& owner);
    CCameraComponent& AddCameraComponent(Core::CGameObject& owner);
    CSpriteComponent& AddSpriteComponent(Core::CGameObject& owner);
    CTransformComponent& AddTransformComponent(Core::CGameObject& owner);
    CMeshComponent& AddMeshComponent(Core::CGameObject& owner);
    void AddComponent(EComponentType type, Core::CGameObject& owner);

    void CloneComponents(Core::CGameObject& dest, const Core::CGameObject& src);

    void Update(float deltaTime);

private:
    struct ComponentPool {
        std::vector<std::unique_ptr<IComponent>> mComponents;
        std::vector<Core::GameObjectId> mEntityIds;
        std::unordered_map<Core::GameObjectId, size_t> mEntityToIndex;
    };

    IComponent* GetComponent(Core::GameObjectId entityId, EComponentType type);

    ComponentPool& GetComponentPool(EComponentType type);
    const ComponentPool& GetComponentPool(EComponentType type) const;

    EComponentType GetComponentType(std::type_index type) const;

    template <typename T, typename... Args>
    T& CreateComponent(Core::CGameObject& owner, Core::GameObjectId id,
                       Args&&... args) {
        auto& pool = GetComponentPool(GetComponentType(typeid(T)));
        auto component =
            std::make_unique<T>(owner, *this, std::forward<Args>(args)...);
        T* rawPtr = component.get();

        size_t newIndex = pool.mComponents.size();
        pool.mComponents.emplace_back(std::move(component));
        pool.mEntityIds.emplace_back(id);
        pool.mEntityToIndex[id] = newIndex;
        return *rawPtr;
    }

    template <typename T>
    T& CreateComponent(Core::CGameObject& owner, Core::GameObjectId id,
                       const T& other) {
        auto& pool = GetComponentPool(GetComponentType(typeid(T)));
        auto component = std::make_unique<T>(owner, *this, other);
        T* rawPtr = component.get();

        size_t newIndex = pool.mComponents.size();
        pool.mComponents.emplace_back(std::move(component));
        pool.mEntityIds.emplace_back(id);
        pool.mEntityToIndex[id] = newIndex;
        return *rawPtr;
    }

    void CreateComponentClone(EComponentType type, Core::CGameObject& owner,
                              Core::GameObjectId id, const IComponent& other);

    Font::CFontHandler& mFontHandler;
    Renderer::CTextRenderer& mTextRenderer;
    Material::CMaterialManager& mMaterialManager;
    Renderer::CSpriteManager& mSpriteManager;
    std::vector<ComponentPool> mComponentPools;
};
} // namespace Component
