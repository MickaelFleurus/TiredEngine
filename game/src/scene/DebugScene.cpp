#include "scene/DebugScene.h"

#include "engine/core/3DCamera.h"

namespace Scene {
CDebugScene::CDebugScene(Component::CComponentManager& componentManager,
                         Font::CFontHandler& fontHandler,
                         Core::CMeshManager& meshManager,
                         const System::CSystem& system)
    : CAbstractScene(componentManager, fontHandler, meshManager, system) {
    mActiveCamera = std::make_unique<Core::C3DCamera>(
        *mSceneRoot, mGameObjectBuilder, mComponentManager);

    // Position camera to look at the cube
    // Camera at (0, 0, -5) looking at (0, 0, 1)
    // auto& cameraTransform =
    //     *mComponentManager.GetComponent<Component::CTransformComponent>(
    //         mActiveCamera->mEntity.GetId());
    // cameraTransform.SetPosition({0.0f, 2.0f, -5.0f});
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
    CreateGameObjectBuilder("Cube!")
        .Add3DCube(1.0f)
        .SetLocalPosition({0.0f, 0.0f, -5.0f})
        .SetMaterialType(Material::EMaterialType::Normal)
        .Build();
    CreateGameObjectBuilder("Cube2!")
        .Add3DCube(3.0f)
        .SetLocalPosition({050.0f, 0.0f, -5.0f})
        .SetMaterialType(Material::EMaterialType::Normal)
        .Build();
    CreateGameObjectBuilder("Cube3!")
        .Add3DCube(2.0f)
        .SetLocalPosition({-50.0f, 0.0f, -5.0f})
        .SetMaterialType(Material::EMaterialType::Normal)
        .Build();
    CreateGameObjectBuilder("TEXT!").AddText("Hello Debug!", 24).Build();
}

void CDebugScene::Unload() {
    // Unload resources specific to the debug scene
}

} // namespace Scene