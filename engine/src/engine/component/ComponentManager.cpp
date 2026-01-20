#include "engine/component/ComponentManager.h"

// #include "engine/component/CollisionComponent.h"
#include <magic_enum/magic_enum.hpp>

#include "engine/component/CameraComponent.h"
#include "engine/component/InputComponent.h"
#include "engine/component/MeshComponent.h"
#include "engine/component/MovementComponent.h"
#include "engine/component/SpriteComponent.h"
#include "engine/component/TextUIComponent.h"
#include "engine/core/GameObject.h"
#include "engine/utils/Logger.h"

namespace Component {
CComponentManager::CComponentManager(
    Font::CFontHandler& fontHandler, Renderer::CTextRenderer& textRenderer,
    Material::CMaterialManager& materialManager,
    Renderer::CSpriteManager& spriteManager,
    Core::CCameraManager& cameraManager)
    : mFontHandler(fontHandler)
    , mTextRenderer(textRenderer)
    , mMaterialManager(materialManager)
    , mSpriteManager(spriteManager)
    , mCameraManager(cameraManager)
    , mComponentPools(magic_enum::enum_count<Component::EComponentType>()) {
}

CSpriteComponent&
CComponentManager::AddSpriteComponent(Core::GameObjectId owner) {
    return CreateComponent<CSpriteComponent>(owner, mSpriteManager);
}

CTextUIComponent&
CComponentManager::AddTextComponent(Core::GameObjectId owner) {
    return CreateComponent<CTextUIComponent>(owner);
}

CCamera3DComponent&
CComponentManager::AddCameraComponent(Core::GameObjectId owner) {
    return CreateComponent<CCamera3DComponent>(owner, mCameraManager);
}

CMeshComponent& CComponentManager::AddMeshComponent(Core::GameObjectId owner) {
    return CreateComponent<CMeshComponent>(owner, mMaterialManager);
}

void CComponentManager::AddComponent(EComponentType type,
                                     Core::GameObjectId owner) {
    switch (type) {
    case EComponentType::Sprite:
        AddSpriteComponent(owner);
        break;
    case EComponentType::TextUI:
        AddTextComponent(owner);
        break;
    case EComponentType::Camera:
        AddCameraComponent(owner);
        break;
    case EComponentType::Mesh:
        AddMeshComponent(owner);
        break;
    }
}

void CComponentManager::RemoveComponents(Core::GameObjectId id) {
    for (auto type : magic_enum::enum_values<Component::EComponentType>()) {
        RemoveComponent(id, type);
    }
}

void CComponentManager::CloneComponents(Core::GameObjectId dest,
                                        Core::GameObjectId src) {
    for (auto type : magic_enum::enum_values<Component::EComponentType>()) {
        if (HasComponent(src, type)) {
            CreateComponentClone(type, dest, *GetComponent(src, type));
        }
    }
}

void CComponentManager::Update(float deltaTime) {
    for (auto& pool : mComponentPools) {
        for (auto& component : pool.mComponents) {
            component->Update(deltaTime);
        }
    }
}

IComponent* CComponentManager::GetComponent(Core::GameObjectId entityId,
                                            EComponentType type) {
    auto& pool = GetComponentPool(type);
    auto it = pool.mEntityToIndex.find(entityId);
    if (it != pool.mEntityToIndex.end()) {
        return pool.mComponents[it->second].get();
    }
    return nullptr;
}

CComponentManager::ComponentPool&
CComponentManager::GetComponentPool(EComponentType type) {
    return mComponentPools[static_cast<int>(type)];
}

const CComponentManager::ComponentPool&
CComponentManager::GetComponentPool(EComponentType type) const {
    return mComponentPools.at(static_cast<int>(type));
}

void CComponentManager::RemoveComponent(Core::GameObjectId id,
                                        EComponentType type) {
    auto& pool = GetComponentPool(type);
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

bool CComponentManager::HasComponent(Core::GameObjectId id,
                                     EComponentType type) const {
    const auto& pool = GetComponentPool(type);
    return pool.mEntityToIndex.contains(id);
}

void CComponentManager::CreateComponentClone(EComponentType type,
                                             Core::GameObjectId id,
                                             const IComponent& other) {
    switch (type) {
    case EComponentType::Sprite:
        CreateComponent<CSpriteComponent>(
            id, static_cast<const CSpriteComponent&>(other));
        break;
    case EComponentType::TextUI:
        CreateComponent<CTextUIComponent>(
            id, static_cast<const CTextUIComponent&>(other));
        break;
    case EComponentType::Mesh:
        CreateComponent<CMeshComponent>(
            id, static_cast<const CMeshComponent&>(other));
        break;
    }
}

EComponentType CComponentManager::GetComponentType(std::type_index type) const {
    if (type == typeid(CSpriteComponent)) {
        return EComponentType::Sprite;
    } else if (type == typeid(CTextUIComponent)) {
        return EComponentType::TextUI;
    } else if (type == typeid(CCamera3DComponent)) {
        return EComponentType::Camera;
    } else if (type == typeid(CMeshComponent)) {
        return EComponentType::Mesh;
    }
    LOG_FATAL("Unknown component type");
    return EComponentType::Sprite; // Default return to avoid compiler
                                   // warning
}
} // namespace Component
