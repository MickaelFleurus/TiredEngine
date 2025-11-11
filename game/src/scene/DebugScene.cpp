#include "scene/DebugScene.h"

namespace Scene {
CDebugScene::CDebugScene(Component::CComponentManager& componentManager,
                         Font::CFontHandler& fontHandler,
                         const System::CSystem& system)
    : CAbstractScene(componentManager, fontHandler, system) {
    CreateGameObjectBuilder("Bibboop").build();
    CreateGameObjectBuilder("TopLeft")
        .addText("TopLeft", 50)
        .setAnchor(Utils::EAnchors::TopLeft)
        .setLocalPosition({0.0f, 1080.0f, 0.0f})
        .build();

    CreateGameObjectBuilder("TopRight")
        .addText("TopRight", 50)
        .setAnchor(Utils::EAnchors::TopRight)
        .setLocalPosition({1920.0f, 1080.0f, 0.0f})
        .build();

    CreateGameObjectBuilder("BottomLeft")
        .addText("BottomLeft", 50)
        .setAnchor(Utils::EAnchors::BottomLeft)
        .setLocalPosition({0.0f, 0.0f, 0.0f})
        .build();

    CreateGameObjectBuilder("BottomRight")
        .addText("BottomRight", 50)
        .setAnchor(Utils::EAnchors::BottomRight)
        .setLocalPosition({1920.0f, 0.0f, 0.0f})
        .build();

    CreateGameObjectBuilder("0,0")
        .addText("X", 50)
        .setAnchor(Utils::EAnchors::Center)
        .setLocalPosition({0.0f, 0.0f, 0.0f})
        .build();

    CreateGameObjectBuilder("1920,0")
        .addText("X", 50)
        .setAnchor(Utils::EAnchors::Center)
        .setLocalPosition({1920.0f, 0.0f, 0.0f})
        .build();

    CreateGameObjectBuilder("1920,1080")
        .addText("X", 50)
        .setAnchor(Utils::EAnchors::Center)
        .setLocalPosition({1920.0f, 1080.0f, 0.0f})
        .build();

    CreateGameObjectBuilder("0,1080")
        .addText("X", 50)
        .setAnchor(Utils::EAnchors::Center)
        .setLocalPosition({0.0f, 1080.0f, 0.0f})
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