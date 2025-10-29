#include "debug/OverlordToolHandler.h"

#include <engine/debug/Overlord.h>

namespace Debug {

COverlordToolHandler::COverlordToolHandler(
    Component::CComponentManager& componentManager,
    Utils::CFileHandler& fileHandler, Scene::CSceneHandler& sceneHandler)
    : mSceneLoaderOverlord(sceneHandler)
    , mEntityWidget(componentManager, fileHandler)
    , mSceneHierarchy(mEntityWidget, sceneHandler) {
}

void COverlordToolHandler::RegisterTools() {
    COverlord::AddMenu(mSceneLoaderOverlord, mToken);

    COverlord::AddWidget(mSceneHierarchy, mToken);
    COverlord::AddWidget(mEntityWidget, mToken);
}

} // namespace Debug