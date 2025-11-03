#pragma once

#include "debug/OverlordToolHandler.h"
#include "scene/SceneHandler.h"

#include <engine/core/EngineLoop.h>

namespace Core {
class CGameLoop : public CEngineLoop {
public:
    CGameLoop();

    void GameLoop(float deltaTime) override;

private:
    Scene::CSceneHandler mSceneHandler;
    Debug::COverlordToolHandler mToolHandler;
};
} // namespace Core