#pragma once

#include "engine/debug/IOverlordItem.h"

namespace Core {
class CCameraUI;
class CCameraManager;
} // namespace Core

namespace Debug {
class CCameraUIWidget : public IOverlordItem {
public:
    CCameraUIWidget(Core::CCameraManager& cameraManager);
    void Render() override;
    const char* GetName() const override;

private:
    Core::CCameraUI& mCamera;
};
} // namespace Debug