#include "engine/core/EntityManager.h"

#include "engine/component/ComponentManager2.h"
#include "engine/component/TransformComponent.h"
#include "engine/core/Entity.h"

namespace Core {

CEntityManager::CEntityManager(Component::CManager& componentManager)
    : mComponentManager(componentManager) {
}

SEntity CEntityManager::Create() {
    uint32_t index;
    if (!mFreeIndices.empty()) {
        index = mFreeIndices.back();
        mFreeIndices.pop_back();
    } else {
        index = static_cast<uint32_t>(mEntityGeneration.size());
        mEntityGeneration.push_back(0);
    }
    SEntity entity{index, mEntityGeneration[index]};

    auto& localTransformPool =
        mComponentManager.GetPool<Component::SLocalTransform>();
    localTransformPool.Create(entity);
    auto& worldMatricePool =
        mComponentManager.GetPool<Component::SLocalTransform>();
    worldMatricePool.Create(entity);
    return entity;
}

void CEntityManager::Destroy(SEntity e) {
    mComponentManager.DestroyComponents(e);
    mEntityGeneration[e.id]++;
    mFreeIndices.push_back(e.id);
}

bool CEntityManager::IsAlive(SEntity e) const {
    return e.id < mEntityGeneration.size() &&
           mEntityGeneration[e.id] == e.generation;
}
} // namespace Core
