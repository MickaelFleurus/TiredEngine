#include "debug/OverlordToolHandler.h"

#include <engine/debug/Overlord.h>

namespace Debug {

COverlordToolHandler::COverlordToolHandler(Scene::CSceneHandler& sceneHandler)
    : mSceneLoaderOverlord(sceneHandler), mSceneHierarchy(sceneHandler) {
}

void COverlordToolHandler::RegisterTools() {
    COverlord::AddMenu(mSceneLoaderOverlord, mToken);

    COverlord::AddWidget(mSceneHierarchy, mToken);
}

} // namespace Debug