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
class AbstractScene {
public:
    explicit AbstractScene(Component::CComponentManager& componentManager,
                           const Core::CWindowData& windowData);
    virtual ~AbstractScene() = default;
    virtual void Update(float deltaTime) = 0;
    virtual AbstractScene* GetNextScene() const = 0;

    Core::CGameObject& GetRoot();

    Core::CGameObjectBuilder::CBuilder
    CreateGameObjectBuilder(Core::CGameObject* parent = nullptr);

protected:
    std::unique_ptr<Core::CGameObject> mSceneRoot;
    Component::CComponentManager& mComponentManager;
    const Core::CWindowData& mWindowData;
    const Input::CInputWatcher mInputWatcher;
    Core::CGameObjectBuilder mGameObjectBuilder;
};
} // namespace Scene