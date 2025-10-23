#pragma once

#include "debug/OverlordToolHandler.h"

#include <engine/core/EngineLoop.h>

namespace Core {
class CGameLoop : public CEngineLoop {
public:
    CGameLoop();

    void GameLoop(float deltaTime) override;

private:
    Debug::COverlordToolHandler mToolHandler;
};
} // namespace Core