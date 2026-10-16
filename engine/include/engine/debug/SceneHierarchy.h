#pragma once

#include <optional>
#include <unordered_set>
#include <vector>

#include "engine/core/Entity.h"
#include "engine/debug/IOverlordItem.h"

namespace Component {
class CManager;
}

namespace Core {
class CEntityManager;
}

namespace Debug {
class CSceneHierarchy : public IOverlordItem {
public:
    CSceneHierarchy(Core::CEntityManager& entityManager,
                    Component::CManager& componentManager);

    void Render() override;
    const char* GetName() const override;

private:
    void RenderEntityNode(Core::SEntity entity,
                          const std::vector<Core::SEntity>& entities,
                          std::unordered_set<uint32_t>& visited);
    void RenderSelectedEntity();

    Core::CEntityManager& mEntityManager;
    Component::CManager& mComponentManager;
    std::optional<Core::SEntity> mSelectedEntity;
};
} // namespace Debug
