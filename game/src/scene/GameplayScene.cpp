#include "scene/GameplayScene.h"

#include <engine/core/Camera3D.h>

#include "core/Grid.h"

namespace Scene {

CGameplayScene::CGameplayScene(Component::CComponentManager& componentManager,
                               Font::CFontHandler& fontHandler,
                               Core::CMeshManager& meshManager,
                               const System::CSystem& system)
    : CAbstractScene(componentManager, fontHandler, meshManager, system) {
    mActiveCamera = std::make_unique<Core::CCamera3D>(
        *mSceneRoot, mGameObjectBuilder, mComponentManager);
}

CGameplayScene::~CGameplayScene() = default;

void CGameplayScene::Update(float deltaTime) {
    // Implementation of Update
}

CAbstractScene* CGameplayScene::GetNextScene() const {
    return nullptr; // No next scene for now
}

const char* CGameplayScene::GetName() const {
    return "Gameplay";
}

void CGameplayScene::Load() {
}

void CGameplayScene::Unload() {
    // Implementation of Unload
}

Core::CGrid& CGameplayScene::CreateGrid(int width, int height) {
    mGrid = std::make_unique<Core::CGrid>(mGameObjectBuilder, mComponentManager,
                                          *mSceneRoot, width, height);
    return *mGrid;
}

} // namespace Scene