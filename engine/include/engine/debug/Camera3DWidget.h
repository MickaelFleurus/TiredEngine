#pragma once

#include "engine/debug/IOverlordItem.h"

namespace Core {
class CCamera3D;
class CCameraManager;
} // namespace Core

namespace Debug {
class CCamera3DWidget : public IOverlordItem {
public:
    CCamera3DWidget(Core::CCameraManager& cameraManager);
    void Render() override;
    const char* GetName() const override;

private:
    Core::CCamera3D& mCamera;
};
} // namespace Debug