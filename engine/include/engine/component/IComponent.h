#pragma once

namespace Core {
class CGameObject;
}

namespace Component {
class CComponentManager;
class IComponent {
public:
    explicit IComponent(Core::CGameObject& owner, CComponentManager& componentManager)
        : mOwner(owner), mComponentManager(componentManager) {}
    virtual ~IComponent() = default;

    virtual void update(float /*deltaTime   */) {}

protected:
    Core::CGameObject& mOwner;
    CComponentManager& mComponentManager;
};
} // namespace Component