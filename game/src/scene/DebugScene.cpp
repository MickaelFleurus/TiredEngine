#include "scene/DebugScene.h"

namespace Scene {
CDebugScene::CDebugScene(Component::CComponentManager& componentManager,
                         Font::CFontHandler& fontHandler,
                         const System::CSystem& system)
    : CAbstractScene(componentManager, fontHandler, system) {
    CreateGameObjectBuilder("Bibboop").build();
    auto* obj = CreateGameObjectBuilder("Bibboop2").build();
    CreateGameObjectBuilder("ChildOfBibboop2", obj)
        .addText("Hello!", 40)
        .setLocalPosition({50.0f, 50.0f, 0.0f})
        .build();
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