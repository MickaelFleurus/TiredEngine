#pragma once

namespace Core {
class CGameObject;
}

namespace Component {
class CComponentManager;
class IComponent {
public:
    explicit IComponent(Core::CGameObject& owner,
                        CComponentManager& componentManager)
        : mOwner(owner), mComponentManager(componentManager) {
    }
    virtual ~IComponent() = default;

    virtual void update(float /*deltaTime   */) {
    }

    bool isDirty() const {
        return mIsDirty;
    }

    virtual void setDirty(bool dirty) {
        mIsDirty = dirty;
    }

protected:
    Core::CGameObject& mOwner;
    CComponentManager& mComponentManager;

    bool mIsDirty{true};
};
} // namespace Component