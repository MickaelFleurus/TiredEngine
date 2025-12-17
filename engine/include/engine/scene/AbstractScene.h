#pragma once
#include "engine/core/GameObjectBuilder.h"
#include "engine/input/InputWatcher.h"

namespace Component {
class CComponentManager;
}

namespace System {
class CSystem;
}

namespace Core {
class CGameObject;
class CCamera;
class CCameraUI;
class CMeshManager;
} // namespace Core

namespace Font {
class CFontHandler;
}

namespace Renderer {
class CMeshRenderer;
}

namespace Scene {
class CAbstractScene {
public:
    explicit CAbstractScene(Component::CComponentManager& componentManager,
                            Font::CFontHandler& fontHandler,
                            Core::CMeshManager& meshManager,
                            const System::CSystem& system);
    virtual ~CAbstractScene();
    virtual void Update(float deltaTime) = 0;
    virtual CAbstractScene* GetNextScene() const = 0;
    virtual void Load() = 0;
    virtual void Unload() = 0;

    Core::CGameObject& GetRoot();
    virtual const char* GetName() const = 0;

    Core::CGameObjectBuilder::CBuilder
    CreateGameObjectBuilder(const std::string& name,
                            Core::CGameObject* parent = nullptr);

    Core::CCamera& GetActiveCamera();
    Core::CCamera& GetUICamera();

protected:
    std::unique_ptr<Core::CGameObject> mSceneRoot;
    std::unique_ptr<Core::CCamera> mActiveCamera;
    std::unique_ptr<Core::CCameraUI> mUICamera;
    Component::CComponentManager& mComponentManager;
    const System::CSystem& mSystem;
    const Input::CInputWatcher mInputWatcher;
    Core::CGameObjectBuilder mGameObjectBuilder;
};
} // namespace Scene
