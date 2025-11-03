#include "scene/DebugScene.h"

namespace Scene {
CDebugScene::CDebugScene(Component::CComponentManager& componentManager,
                         const Core::CWindowData& windowData)
    : CAbstractScene(componentManager, windowData) {
    CreateGameObjectBuilder("Bibboop").build();
    auto* obj = CreateGameObjectBuilder("Bibboop2").build();
    CreateGameObjectBuilder("ChildOfBibboop2", obj).build();
}

void CDebugScene::Update(float deltaTime) {
    // Debug scene update logic goes here
}

CAbstractScene* CDebugScene::GetNextScene() const {
    return nullptr; // No next scene for debug scene
}

const char* CDebugScene::GetName() const {
    return "CDebugScene";
}

} // namespace Scene