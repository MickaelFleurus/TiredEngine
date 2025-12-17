#pragma once
#include "engine/core/Camera.h"

namespace Core {
class CCameraUI : public CCamera {
public:
    CCameraUI(CGameObject& parent, CGameObjectBuilder& builder,
              Component::CComponentManager& componentManager);

protected:
    void EnsureUpToDate() override;

private:
};
} // namespace Core
