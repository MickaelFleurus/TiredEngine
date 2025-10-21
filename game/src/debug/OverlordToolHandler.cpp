#include "debug/OverlordToolHandler.h"

#include <engine/debug/Overlord.h>

namespace Debug {

COverlordToolHandler::COverlordToolHandler(Scene::ISceneLoader& sceneLoader)
    : mSceneLoaderOverlord(sceneLoader), mToken() {
}

void COverlordToolHandler::RegisterTools() {
    COverlord::AddMenu(mSceneLoaderOverlord, mToken);
}

} // namespace Debug