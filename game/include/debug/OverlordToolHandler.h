#pragma once

#include "debug/SceneLoaderOverlord.h"
#include <engine/utils/Token.h>

namespace Scene {
class ISceneLoader;
} // namespace Scene

namespace Debug {
class COverlordToolHandler {
public:
    COverlordToolHandler(Scene::ISceneLoader& sceneLoader);

    void RegisterTools();

private:
    CSceneLoaderOverlord mSceneLoaderOverlord;
    CToken mToken;
};
} // namespace Debug