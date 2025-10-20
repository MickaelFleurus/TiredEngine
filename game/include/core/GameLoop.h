#pragma once

#include "engine/core/EngineLoop.h"

namespace Core {
class CGameLoop : public CEngineLoop {
public:
    CGameLoop();

    void GameLoop(float deltaTime) override;
};
} // namespace Core