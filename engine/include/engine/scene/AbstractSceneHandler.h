#pragma once
#include "engine/scene/ISceneHandler.h"

#include <memory>
#include <span>

namespace Component {
class CComponentManager;
}

namespace Core {
class CWindowData;
class CEngineLoop;
} // namespace Core

namespace Scene {

class CAbstractSceneHandler : public ISceneHandler {
public:
    explicit CAbstractSceneHandler(
        Core::CEngineLoop& engineLoop,
        Component::CComponentManager& componentManager,
        Core::CWindowData& windowData);

    CAbstractScene* GetCurrentScene() const override;

    const std::vector<std::string>& GetSceneNames() const override;

protected:
    template <typename GameSceneType, typename... Args>
    void CreateAndSetSceneImpl(Args&&... args) {
        static_assert(std::is_base_of_v<CAbstractScene, GameSceneType>,
                      "GameSceneType must derive from CAbstractScene");

        auto scene = std::make_unique<GameSceneType>(
            mComponentManager, mWindowData, std::forward<Args>(args)...);
        SetCurrentScene(std::move(scene));
    }
    void SetCurrentScene(std::unique_ptr<Scene::CAbstractScene>&& scene);

    const std::vector<std::string> mSceneNames;
    Core::CEngineLoop& mEngineLoop;
    Component::CComponentManager& mComponentManager;
    Core::CWindowData& mWindowData;
};
} // namespace Scene