#pragma once
#include "engine/core/GameObjectBuilder.h"
#include "engine/input/InputWatcher.h"

namespace Component {
class CComponentManager;
}

namespace Core {
class CGameObject;
struct SWindowData;
} // namespace Core

namespace Scene {
class AbstractScene {
public:
    explicit AbstractScene(Component::CComponentManager& componentManager,
                           const Core::SWindowData& windowData);
    virtual ~AbstractScene() = default;
    virtual void update(float deltaTime) = 0;
    virtual AbstractScene* getNextScene() const = 0;

    Core::CGameObject& getRoot();

    Core::CGameObjectBuilder::CBuilder
    createGameObjectBuilder(Core::CGameObject* parent = nullptr);

protected:
    std::unique_ptr<Core::CGameObject> mSceneRoot;
    Component::CComponentManager& mComponentManager;
    const Core::SWindowData& mWindowData;
    const Input::CInputWatcher mInputWatcher;
    Core::CGameObjectBuilder mGameObjectBuilder;
};
} // namespace Scene