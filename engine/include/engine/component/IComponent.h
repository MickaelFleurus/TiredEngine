#pragma once

#include "engine/core/DirtyTypeEnum.h"
#include "engine/core/GameObjectId.h"

namespace Component {
class CComponentManager;
class IComponent {
public:
    explicit IComponent(Core::GameObjectId objId,
                        CComponentManager& componentManager)
        : mId(objId), mComponentManager(componentManager) {
    }
    virtual ~IComponent() = default;

    virtual void Update(float /*deltaTime   */) {
    }

    bool IsDirty() const {
        return mIsDirty != Core::EDirtyFlag::None;
    }

    void AddDirtyFlag(Core::EDirtyFlag type) {
        mIsDirty = mIsDirty | type;
    }

    void Clean() {
        mIsDirty = Core::EDirtyFlag::None;
    }

    Core::EDirtyFlag GetDirtyFlag() const {
        return mIsDirty;
    }

    Core::GameObjectId GetId() const {
        return mId;
    }

protected:
    Core::GameObjectId mId;
    CComponentManager& mComponentManager;

    Core::EDirtyFlag mIsDirty{Core::EDirtyFlag::New};
};
} // namespace Component
