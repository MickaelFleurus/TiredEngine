#pragma once

#include <vector>

#include "engine/core/Entity.h"

namespace Component {
class CManager;
}

namespace Core {
class CEntityManager {
public:
    explicit CEntityManager(Component::CManager& componentManager);
    SEntity Create();

    void Destroy(SEntity e);

    bool IsAlive(SEntity e) const;

private:
    std::vector<uint32_t> mEntityGeneration;
    std::vector<uint32_t> mFreeIndices;
    Component::CManager& mComponentManager;
};
} // namespace Core
