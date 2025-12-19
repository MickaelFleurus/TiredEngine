#include "scene/DebugScene.h"

#include "engine/core/Camera3D.h"

namespace Scene {
CDebugScene::CDebugScene(Component::CComponentManager& componentManager,
                         Font::CFontHandler& fontHandler,
                         Core::CMeshManager& meshManager,
                         const System::CSystem& system)
    : CAbstractScene(componentManager, fontHandler, meshManager, system) {
    mActiveCamera = std::make_unique<Core::CCamera3D>(
        *mSceneRoot, mGameObjectBuilder, mComponentManager);
    mLoadCallback = [](CAbstractScene& scene) {
        scene.CreateGameObjectBuilder("Cube!")
            .Add3DCube(1.0f)
            .SetLocalPosition({0.0f, 0.0f, -5.0f})
            .SetMaterialType(Material::EMaterialType::Normal)
            .Build();
        scene.CreateGameObjectBuilder("Cube2!")
            .Add3DCube(3.0f)
            .SetLocalPosition({050.0f, 0.0f, -5.0f})
            .SetMaterialType(Material::EMaterialType::Normal)
            .Build();
        scene.CreateGameObjectBuilder("Cube3!")
            .Add3DCube(2.0f)
            .SetLocalPosition({-50.0f, 0.0f, -5.0f})
            .SetMaterialType(Material::EMaterialType::Normal)
            .Build();
        scene.CreateGameObjectBuilder("TEXT!")
            .AddText("Hello Debug! \nYo", 60)
            .Build();
    };
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
    mLoadCallback(*this);
}

void CDebugScene::Unload() {
    // Unload resources specific to the debug scene
}

} // namespace Scene
