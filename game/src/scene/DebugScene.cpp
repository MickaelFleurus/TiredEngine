#include "scene/DebugScene.h"

namespace Scene {
CDebugScene::CDebugScene(Component::CComponentManager& componentManager,
                         Font::CFontHandler& fontHandler,
                         Core::CMeshManager& meshManager,
                         const System::CSystem& system)
    : CAbstractScene(componentManager, fontHandler, meshManager, system) {
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

void CDebugScene::Load() {
    CreateGameObjectBuilder("Bibboop").Build();
    CreateGameObjectBuilder("TopLeft")
        .AddText("TopLeft", 50)
        .SetAnchor(Utils::EAnchors::TopLeft)
        .SetLocalPosition({0.0f, 1080.0f, 0.0f})
        .Build();

    CreateGameObjectBuilder("TopRight")
        .AddText("TopRight", 50)
        .SetAnchor(Utils::EAnchors::TopRight)
        .SetLocalPosition({1920.0f, 1080.0f, 0.0f})
        .Build();

    CreateGameObjectBuilder("BottomLeft")
        .AddText("BottomLeft", 50)
        .SetAnchor(Utils::EAnchors::BottomLeft)
        .SetLocalPosition({0.0f, 0.0f, 0.0f})
        .Build();

    CreateGameObjectBuilder("BottomRight")
        .AddText("BottomRight", 50)
        .SetAnchor(Utils::EAnchors::BottomRight)
        .SetLocalPosition({1920.0f, 0.0f, 0.0f})
        .Build();

    CreateGameObjectBuilder("0,0")
        .AddText("X", 50)
        .SetAnchor(Utils::EAnchors::Center)
        .SetLocalPosition({0.0f, 0.0f, 0.0f})
        .Build();

    CreateGameObjectBuilder("1920,0")
        .AddText("X", 50)
        .SetAnchor(Utils::EAnchors::Center)
        .SetLocalPosition({1920.0f, 0.0f, 0.0f})
        .Build();

    CreateGameObjectBuilder("1920,1080")
        .AddText("X", 50)
        .SetAnchor(Utils::EAnchors::Center)
        .SetLocalPosition({1920.0f, 1080.0f, 0.0f})
        .Build();

    CreateGameObjectBuilder("0,1080")
        .AddText("X", 50)
        .SetAnchor(Utils::EAnchors::Center)
        .SetLocalPosition({0.0f, 1080.0f, 0.0f})
        .Build();

    CreateGameObjectBuilder("Cube!")
        .Add3DCube(1.0f)
        .SetLocalPosition({0.0f, 0.0f, -5.0f})
        .Build();
}

void CDebugScene::Unload() {
    // Unload resources specific to the debug scene
}

} // namespace Scene