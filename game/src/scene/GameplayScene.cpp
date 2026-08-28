#include "scene/GameplayScene.h"

#include <engine/core/Camera3D.h>

#include "core/Grid.h"

namespace Scene {

CGameplayScene::CGameplayScene(const System::CSystem& system)
    : CAbstractScene() {
}

CGameplayScene::~CGameplayScene() = default;

void CGameplayScene::Update(float deltaTime) {
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
}

// Core::CGrid& CGameplayScene::CreateGrid(int width, int height) {
//     mGrid = std::make_unique<Core::CGrid>(mGameObjectBuilder,
//     mComponentManager,
//                                           *mSceneRoot, width, height);
//     return *mGrid;
// }

} // namespace Scene
