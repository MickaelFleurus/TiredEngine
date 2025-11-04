#pragma once
#include "engine/core/GameObjectBuilder.h"
#include "engine/input/InputWatcher.h"

namespace Component {
class CComponentManager;
}

namespace Core {
class CGameObject;
class CWindowData;
class CCamera;
} // namespace Core

namespace Font {
class CFontHandler;
}

namespace Scene {
class CAbstractScene {
public:
    explicit CAbstractScene(Component::CComponentManager& componentManager,
                            Font::CFontHandler& fontHandler,
                            const Core::CWindowData& windowData);
    virtual ~CAbstractScene();
    virtual void Update(float deltaTime) = 0;
    virtual CAbstractScene* GetNextScene() const = 0;

    Core::CGameObject& GetRoot();
    virtual const char* GetName() const = 0;

    Core::CGameObjectBuilder::CBuilder
    CreateGameObjectBuilder(const std::string& name,
                            Core::CGameObject* parent = nullptr);

    Core::CCamera& GetActiveCamera();

protected:
    std::unique_ptr<Core::CGameObject> mSceneRoot;
    std::unique_ptr<Core::CCamera> mActiveCamera;
    Component::CComponentManager& mComponentManager;
    const Core::CWindowData& mWindowData;
    const Input::CInputWatcher mInputWatcher;
    Core::CGameObjectBuilder mGameObjectBuilder;
};
} // namespace Scene