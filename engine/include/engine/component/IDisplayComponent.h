#pragma once

#include "engine/component/IComponent.h"

#include <glm/vec2.hpp>

namespace Component {
class IDisplayComponent : public IComponent {
public:
    explicit IDisplayComponent(Core::CGameObject& owner,
                               CComponentManager& componentManager,
                               Core::EDirtyType dirtyType)
        : IComponent(owner, componentManager, dirtyType) {
    }
    ~IDisplayComponent() override = default;

    virtual glm::vec2 GetSize() = 0;
};
} // namespace Component