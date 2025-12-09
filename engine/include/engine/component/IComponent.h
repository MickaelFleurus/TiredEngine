#pragma once
#include "engine/core/DirtyTypeEnum.h"
#include "engine/core/GameObject.h"

namespace Component {
class CComponentManager;
class IComponent {
public:
    explicit IComponent(Core::CGameObject& owner,
                        CComponentManager& componentManager,
                        Core::EDirtyType dirtyType)
        : mOwner(owner)
        , mComponentManager(componentManager)
        , mDirtyType(dirtyType) {
    }
    virtual ~IComponent() = default;

    virtual void Update(float /*deltaTime   */) {
    }

    bool IsDirty() const {
        return mIsDirty;
    }

    virtual void SetDirty(bool dirty) {
        mIsDirty = dirty;
        if (dirty) {
            mOwner.AddDirtyFlag(mDirtyType);
        }
    }

    virtual void AddDirtyFlag(Core::EDirtyType flag) {
        mIsDirty = flag != Core::EDirtyType::None;
        mOwner.AddDirtyFlag(flag);
    }

protected:
    Core::CGameObject& mOwner;
    CComponentManager& mComponentManager;

    bool mIsDirty{true};
    const Core::EDirtyType mDirtyType;
};
} // namespace Component