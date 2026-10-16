#include "engine/core/EntityManager.h"

#include "engine/component/ComponentManager2.h"
#include "engine/component/TransformComponent.h"
#include "engine/core/Entity.h"

namespace Core {

CEntityManager::CEntityManager(Component::CManager& componentManager)
    : mComponentManager(componentManager) {
}

SEntity CEntityManager::Create(CStringId name) {
    uint32_t index;
    if (!mFreeIndices.empty()) {
        index = mFreeIndices.back();
        mFreeIndices.pop_back();
    } else {
        index = static_cast<uint32_t>(mEntityGeneration.size());
        mEntityGeneration.push_back(0);
    }
    mEntityNames[name] = index;
    SEntity entity{name, index, mEntityGeneration[index]};

    auto& localTransformPool =
        mComponentManager.GetPool<Component::SLocalTransform>();
    localTransformPool.Create(entity);
    auto& worldMatricePool =
        mComponentManager.GetPool<Component::SWorldTransform>();
    worldMatricePool.Create(entity);
    return entity;
}

void CEntityManager::Destroy(SEntity e) {
    if (!IsAlive(e)) {
        return;
    }
    mComponentManager.DestroyComponents(e);
    const auto it = mEntityNames.find(e.name);
    if (it != mEntityNames.end() && it->second == e.id) {
        mEntityNames.erase(it);
    }
    mEntityGeneration[e.id]++;
    mFreeIndices.push_back(e.id);
}

bool CEntityManager::IsAlive(SEntity e) const {
    return e.id < mEntityGeneration.size() &&
           mEntityGeneration[e.id] == e.generation;
}

std::optional<SEntity> CEntityManager::GetEntity(CStringId name) const {
    auto it = mEntityNames.find(name);
    if (it != mEntityNames.end()) {
        uint32_t index = it->second;
        return SEntity{name, index, mEntityGeneration[index]};
    }
    return std::nullopt;
}

std::vector<SEntity> CEntityManager::GetEntities() const {
    std::vector<SEntity> entities;
    entities.reserve(mEntityNames.size());
    for (const auto& [name, id] : mEntityNames) {
        if (id < mEntityGeneration.size()) {
            entities.push_back({name, id, mEntityGeneration[id]});
        }
    }
    return entities;
}

} // namespace Core
