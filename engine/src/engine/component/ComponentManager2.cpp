#include "engine/component/ComponentManager2.h"

#include "engine/component/ComponentPool.h"

namespace Component {

void CManager::DestroyComponents(Core::SEntity entity) {
    for (auto& pool : mPools) {
        pool->Remove(entity);
    }
}
} // namespace Component
