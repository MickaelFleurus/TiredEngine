#include "engine/core/EntityManager.h"

#include "engine/component/ComponentManager2.h"

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
    return SEntity{index, mEntityGeneration[index]};
}

void CEntityManager::Destroy(SEntity e) {
    mEntityGeneration[e.id]++;
    mFreeIndices.push_back(e.id);
}

bool CEntityManager::IsAlive(SEntity e) const {
    return e.id < mEntityGeneration.size() &&
           mEntityGeneration[e.id] == e.generation;
}
} // namespace Core
