#pragma once
#include "engine/core/GameObjectBuilder.h"
#include "engine/input/InputWatcher.h"

namespace Component {
class CComponentManager;
}

namespace Core {
class CGameObject;
class CWindowData;
} // namespace Core

namespace Scene {
class CAbstractScene {
public:
    explicit CAbstractScene(Component::CComponentManager& componentManager,
                            const Core::CWindowData& windowData);
    virtual ~CAbstractScene() = default;
    virtual void Update(float deltaTime) = 0;
    virtual CAbstractScene* GetNextScene() const = 0;

    Core::CGameObject& GetRoot();
    virtual const char* GetName() const = 0;

    Core::CGameObjectBuilder::CBuilder
    CreateGameObjectBuilder(const std::string& name,
                            Core::CGameObject* parent = nullptr);

protected:
    std::unique_ptr<Core::CGameObject> mSceneRoot;
    Component::CComponentManager& mComponentManager;
    const Core::CWindowData& mWindowData;
    const Input::CInputWatcher mInputWatcher;
    Core::CGameObjectBuilder mGameObjectBuilder;
};
} // namespace Scene