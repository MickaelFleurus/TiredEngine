#pragma once

#include "engine/debug/IOverlordItem.h"

namespace Component {
class CCameraComponent;
} // namespace Component

namespace Debug {
class CCameraWidget : public IOverlordItem {
public:
    CCameraWidget(Component::CCameraComponent& cameraComponent);
    void Render() override;
    const char* GetName() const override;

private:
    Component::CCameraComponent& mCameraComponent;
};
} // namespace Debug