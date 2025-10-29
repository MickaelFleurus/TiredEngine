#pragma once

#include "engine/debug/IOverlordItem.h"

namespace Component {
class CTransformComponent;
}

namespace Debug {

class CTransformComponentWidget : public IOverlordItem {
public:
    CTransformComponentWidget(
        Component::CTransformComponent& transformComponent);
    void Render() override;
    const char* GetName() const override;

private:
    Component::CTransformComponent& mTransformComponent;
};
} // namespace Debug