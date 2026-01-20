#pragma once

#include "engine/core/GameObjectId.h"

namespace Component {
class CComponentManager;
class IComponent {
public:
    explicit IComponent(Core::GameObjectId& objId,
                        CComponentManager& componentManager)
        : mId(objId), mComponentManager(componentManager) {
    }
    virtual ~IComponent() = default;

    virtual void Update(float /*deltaTime   */) {
    }

    bool IsDirty() const {
        return mIsDirty;
    }

    virtual void SetDirty(bool dirty) {
        mIsDirty = dirty;
    }

protected:
    Core::GameObjectId& mId;
    CComponentManager& mComponentManager;

    bool mIsDirty{true};
};
} // namespace Component
