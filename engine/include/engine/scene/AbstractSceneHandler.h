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

namespace Font {
class CFontHandler;
}

namespace Scene {

class CAbstractSceneHandler : public ISceneHandler {
public:
    explicit CAbstractSceneHandler(
        Core::CEngineLoop& engineLoop,
        Component::CComponentManager& componentManager,
        Font::CFontHandler& fontHandler, Core::CWindowData& windowData);

    CAbstractScene* GetCurrentScene() const override;

protected:
    template <typename GameSceneType, typename... Args>
    void CreateAndSetSceneImpl(Args&&... args) {
        static_assert(std::is_base_of_v<CAbstractScene, GameSceneType>,
                      "GameSceneType must derive from CAbstractScene");

        auto scene = std::make_unique<GameSceneType>(
            mComponentManager, mFontHandler, mWindowData,
            std::forward<Args>(args)...);
        SetCurrentScene(std::move(scene));
    }
    void SetCurrentScene(std::unique_ptr<Scene::CAbstractScene>&& scene);

    Core::CEngineLoop& mEngineLoop;
    Component::CComponentManager& mComponentManager;
    Font::CFontHandler& mFontHandler;
    Core::CWindowData& mWindowData;
};
} // namespace Scene