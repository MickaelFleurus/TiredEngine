#include "scene/DebugScene.h"

namespace Scene {
DebugScene::DebugScene(Component::CComponentManager& componentManager,
                       const Core::CWindowData& windowData)
    : CAbstractScene(componentManager, windowData) {
    CreateGameObjectBuilder("Bibboop").build();
    auto* obj = CreateGameObjectBuilder("Bibboop2").build();
    CreateGameObjectBuilder("ChildOfBibboop2", obj).build();
}

void DebugScene::Update(float deltaTime) {
    // Debug scene update logic goes here
}

CAbstractScene* DebugScene::GetNextScene() const {
    return nullptr; // No next scene for debug scene
}

const char* DebugScene::GetName() const {
    return "DebugScene";
}

} // namespace Scene