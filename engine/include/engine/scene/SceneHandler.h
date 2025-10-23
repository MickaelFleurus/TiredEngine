#pragma once

#include <memory>

namespace Component {
class CComponentManager;
}

namespace Core {
class CWindowData;
class CEngineLoop;
} // namespace Core

namespace Scene {

class CAbstractScene;
class CSceneHandler {
public:
    explicit CSceneHandler(Core::CEngineLoop& engineLoop,
                           Component::CComponentManager& componentManager,
                           Core::CWindowData& windowData);

    template <typename GameSceneType, typename... Args>
    void CreateAndSetScene(Args&&... args) {
        static_assert(std::is_base_of_v<CAbstractScene, GameSceneType>,
                      "GameSceneType must derive from CAbstractScene");

        auto scene = std::make_unique<GameSceneType>(
            mComponentManager, mWindowData, std::forward<Args>(args)...);
        SetCurrentScene(std::move(scene));
    }

    CAbstractScene* GetCurrentScene() const;

protected:
    void SetCurrentScene(std::unique_ptr<Scene::CAbstractScene>&& scene);

    Core::CEngineLoop& mEngineLoop;
    Component::CComponentManager& mComponentManager;
    Core::CWindowData& mWindowData;
};
} // namespace Scene