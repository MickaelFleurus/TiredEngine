#pragma once

#include "debug/SceneLoaderOverlord.h"
#include <engine/debug/SceneHierarchy.h>
#include <engine/utils/Token.h>

namespace Scene {
class CSceneHandler;
} // namespace Scene

namespace Debug {
class COverlordToolHandler {
public:
    COverlordToolHandler(Scene::CSceneHandler& sceneLoader);

    void RegisterTools();

private:
    CSceneLoaderOverlord mSceneLoaderOverlord;
    CSceneHierarchy mSceneHierarchy;

    CToken mToken;
};
} // namespace Debug