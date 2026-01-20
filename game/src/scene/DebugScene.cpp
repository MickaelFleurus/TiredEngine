#include "scene/DebugScene.h"

#include "engine/core/Camera3D.h"

namespace Scene {
CDebugScene::CDebugScene(Component::CComponentManager& componentManager,
                         Font::CFontHandler& fontHandler,
                         Core::CMeshManager& meshManager,
                         const System::CSystem& system,
                         Core::CGameObjectManager& gameObjectManager)
    : CAbstractScene(componentManager, fontHandler, meshManager, system,
                     gameObjectManager) {
    mLoadCallback = [this](CAbstractScene& scene) {};
    //     scene.CreateGameObjectBuilder("Camera")
    //         .SetLocalPosition({0.0f, 0.0f, -15.0f})
    //         .AddCameraComponent()
    //         .Build();
    //     scene.CreateGameObjectBuilder("Cube!")
    //         .Add3DCube(1.0f)
    //         .SetLocalPosition({0.0f, 0.0f, -5.0f})
    //         .SetMaterialType(Material::EMaterialType::Normal)
    //         .Build();
    //     scene.CreateGameObjectBuilder("Cube2!")
    //         .Add3DCube(3.0f)
    //         .SetLocalPosition({50.0f, 0.0f, -5.0f})
    //         .SetMaterialType(Material::EMaterialType::Normal)
    //         .Build();
    //     scene.CreateGameObjectBuilder("Cube3!")
    //         .Add3DCube(2.0f)
    //         .SetLocalPosition({-50.0f, 0.0f, -5.0f})
    //         .SetMaterialType(Material::EMaterialType::Normal)
    //         .Build();
    //     auto* obj = scene.CreateGameObjectBuilder("TEXT!")
    //                     .AddText("Hello Debug! \nYo", 60)
    //                     .Build();
    //     auto* clone = obj->Clone(mSceneRoot.get());
    // };
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
