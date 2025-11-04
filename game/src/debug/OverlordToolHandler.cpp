#include "debug/OverlordToolHandler.h"

#include <engine/debug/Overlord.h>

namespace Debug {

COverlordToolHandler::COverlordToolHandler(
    Component::CComponentManager& componentManager,
    Utils::CFileHandler& fileHandler, Scene::ISceneHandler& sceneHandler,
    Font::CFontHandler& fontHandler)
    : mSettings(fileHandler, sceneHandler)
    , mOverlordSettings(mSettings, fileHandler, sceneHandler)
    , mSceneLoaderOverlord(sceneHandler)
    , mEntityWidget(componentManager, fileHandler, fontHandler)
    , mSceneHierarchy(mEntityWidget, sceneHandler) {
}

void COverlordToolHandler::RegisterTools() {
    COverlord::AddMenu(mSceneLoaderOverlord, mToken);

    COverlord::AddWidget(mSceneHierarchy, mToken);
    COverlord::AddWidget(mEntityWidget, mToken);

    COverlord::AddMenu(mOverlordSettings, mToken);
}

} // namespace Debug