#pragma once

#include <vector>

#include "engine/core/Entity.h"
#include "engine/utils/StringId.h"

namespace Component {
class CManager;
}

namespace Core {
class CEntityManager {
public:
    explicit CEntityManager(Component::CManager& componentManager);
    SEntity Create(CStringId name);

    void Destroy(SEntity e);

    bool IsAlive(SEntity e) const;

    SEntity GetEntity(CStringId name) const;

private:
    std::vector<uint32_t> mEntityGeneration;
    std::vector<uint32_t> mFreeIndices;
    std::unordered_map<CStringId, uint32_t, CStringIdHash> mEntityNames;
    Component::CManager& mComponentManager;
};
} // namespace Core
