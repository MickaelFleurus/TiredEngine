#include "engine/scene/AbstractScene.h"

#include "engine/component/CameraComponent.h"
#include "engine/core/CameraUI.h"
#include "engine/system/System.h"

namespace Scene {

CAbstractScene::CAbstractScene(Component::CComponentManager& componentManager,
                               Font::CFontHandler& fontHandler,
                               Core::CMeshManager& meshManager,
                               const System::CSystem& system,
                               Core::CGameObjectManager& gameObjectManager)
    : mGameObjectBuilder(componentManager, fontHandler, meshManager,
                         gameObjectManager) {
}

CAbstractScene::~CAbstractScene() = default;

Core::CGameObjectBuilder& CAbstractScene::GetObjectBuilder() {
    return mGameObjectBuilder;
}

} // namespace Scene
