#pragma once

#include "debug/OverlordToolHandler.h"
#include "scene/ISceneLoader.h"

#include <engine/core/EngineLoop.h>

namespace Core {
class CGameLoop : public CEngineLoop, public Scene::ISceneLoader {
public:
    CGameLoop();

    void GameLoop(float deltaTime) override;

    void LoadScene(Scene::ESceneType sceneType) override;

private:
    Debug::COverlordToolHandler mToolHandler;
};
} // namespace Core