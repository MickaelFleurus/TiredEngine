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

    void SetDirtyFlag(Core::EDirtyFlag type) {
        mIsDirty = type;
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

    bool IsActive() const {
        return mActive;
    }

    void SetActive(bool active) {
        if (active != mActive) {
            AddDirtyFlag(Core::EDirtyFlag::Visibility);
            mActive = active;
        }
    }

protected:
    Core::GameObjectId mId;
    CComponentManager& mComponentManager;
    bool mActive{true};

    Core::EDirtyFlag mIsDirty{Core::EDirtyFlag::New};
};
} // namespace Component
