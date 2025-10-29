#pragma once

#include "debug/SceneLoaderOverlord.h"
#include <engine/debug/EntityWidget.h>
#include <engine/debug/SceneHierarchy.h>
#include <engine/utils/Token.h>

namespace Utils {
class CFileHandler;
}

namespace Scene {
class CSceneHandler;
} // namespace Scene

namespace Component {
class CComponentManager;
} // namespace Component

namespace Debug {
class COverlordToolHandler {
public:
    COverlordToolHandler(Component::CComponentManager& componentManager,
                         Utils::CFileHandler& fileHandler,
                         Scene::CSceneHandler& sceneLoader);

    void RegisterTools();

private:
    CSceneLoaderOverlord mSceneLoaderOverlord;
    CEntityWidget mEntityWidget;
    CSceneHierarchy mSceneHierarchy;

    CToken mToken;
};
} // namespace Debug