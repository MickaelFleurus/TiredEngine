#pragma once

#include <engine/debug/EntityWidget.h>
#include <engine/debug/OverlordSettingsWidget.h>
#include <engine/debug/SceneHierarchy.h>
#include <engine/debug/SceneLoaderOverlord.h>
#include <engine/utils/OverlordSettings.h>
#include <engine/utils/Token.h>

namespace Utils {
class CFileHandler;
}

namespace Scene {
class ISceneHandler;
} // namespace Scene

namespace Component {
class CComponentManager;
} // namespace Component

namespace Debug {
class COverlordToolHandler {
public:
    COverlordToolHandler(Component::CComponentManager& componentManager,
                         Utils::CFileHandler& fileHandler,
                         Scene::ISceneHandler& sceneLoader);

    void RegisterTools();

private:
    Utils::COverlordSettings mSettings;

    COverlordSettingsWidget mOverlordSettings;
    CSceneLoaderOverlord mSceneLoaderOverlord;
    CEntityWidget mEntityWidget;
    CSceneHierarchy mSceneHierarchy;

    CToken mToken;
};
} // namespace Debug