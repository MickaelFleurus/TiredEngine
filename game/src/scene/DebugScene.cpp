#include "scene/DebugScene.h"

namespace Scene {
DebugScene::DebugScene(Component::CComponentManager& componentManager,
                       const Core::CWindowData& windowData)
    : AbstractScene(componentManager, windowData) {
}

void DebugScene::Update(float deltaTime) {
    // Debug scene update logic goes here
}

AbstractScene* DebugScene::GetNextScene() const {
    return nullptr; // No next scene for debug scene
}

} // namespace Scene