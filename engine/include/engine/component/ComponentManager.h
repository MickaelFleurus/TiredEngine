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
class CCameraManager;
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
class CCamera3DComponent;
class CTextUIComponent;
class CMovementComponent;
class CMeshComponent;

class CComponentManager {
public:
    CComponentManager(Font::CFontHandler& fontHandler,
                      Renderer::CTextRenderer& textRenderer,
                      Material::CMaterialManager& materialManager,
                      Renderer::CSpriteManager& spriteManager,
                      Core::CCameraManager& cameraManager);

    template <typename T>
    T* GetComponent(Core::GameObjectId entityId) {
        return static_cast<T*>(
            GetComponent(entityId, GetComponentType(typeid(T))));
    }

    bool HasComponent(Core::GameObjectId entityId, EComponentType type) const;

    void RemoveComponent(Core::GameObjectId id, EComponentType type);
    void RemoveComponents(Core::GameObjectId id);

    CTextUIComponent& AddTextComponent(Core::GameObjectId owner);
    CCamera3DComponent& AddCameraComponent(Core::GameObjectId owner);
    CSpriteComponent& AddSpriteComponent(Core::GameObjectId owner);
    CMeshComponent& AddMeshComponent(Core::GameObjectId owner);
    void AddComponent(EComponentType type, Core::GameObjectId owner);

    void Update(float deltaTime);
    void CloneComponents(Core::GameObjectId dest, Core::GameObjectId src);

private:
    struct ComponentPool {
        std::vector<std::unique_ptr<IComponent>> mComponents;
        std::vector<Core::GameObjectId> mEntityIds;
        std::unordered_map<Core::GameObjectId, size_t, Core::GameObjectIdHash>
            mEntityToIndex;
    };

    IComponent* GetComponent(Core::GameObjectId entityId, EComponentType type);

    ComponentPool& GetComponentPool(EComponentType type);
    const ComponentPool& GetComponentPool(EComponentType type) const;

    EComponentType GetComponentType(std::type_index type) const;

    template <typename T, typename... Args>
    T& CreateComponent(Core::GameObjectId id, Args&&... args) {
        auto& pool = GetComponentPool(GetComponentType(typeid(T)));
        auto component =
            std::make_unique<T>(id, *this, std::forward<Args>(args)...);
        T* rawPtr = component.get();

        size_t newIndex = pool.mComponents.size();
        pool.mComponents.emplace_back(std::move(component));
        pool.mEntityIds.emplace_back(id);
        pool.mEntityToIndex[id] = newIndex;
        return *rawPtr;
    }

    template <typename T>
    T& CreateComponent(Core::GameObjectId id, const T& other) {
        auto& pool = GetComponentPool(GetComponentType(typeid(T)));
        auto component = std::make_unique<T>(id, *this, other);
        T* rawPtr = component.get();

        size_t newIndex = pool.mComponents.size();
        pool.mComponents.emplace_back(std::move(component));
        pool.mEntityIds.emplace_back(id);
        pool.mEntityToIndex[id] = newIndex;
        return *rawPtr;
    }

    void CreateComponentClone(EComponentType type, Core::GameObjectId id,
                              const IComponent& other);

    Font::CFontHandler& mFontHandler;
    Renderer::CTextRenderer& mTextRenderer;
    Material::CMaterialManager& mMaterialManager;
    Renderer::CSpriteManager& mSpriteManager;
    Core::CCameraManager& mCameraManager;
    std::vector<ComponentPool> mComponentPools;
};
} // namespace Component
