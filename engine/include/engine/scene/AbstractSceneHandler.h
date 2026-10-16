#pragma once
#include <memory>

#include "engine/scene/ISceneHandler.h"

namespace System {
class CSystem;
}

namespace Component {
class CComponentManager;
}

namespace Core {
class CWindowData;
class CEngineLoop;
class CMeshManager;
class CGameObjectManager;
} // namespace Core

namespace Font {
class CFontHandler;
}

namespace Scene {

class CAbstractSceneHandler : public ISceneHandler {
public:
    explicit CAbstractSceneHandler(Core::CEngineLoop& engineLoop,
                                   const System::CSystem& system);

    CAbstractScene* GetCurrentScene() const override;

protected:
    template <typename GameSceneType, typename... Args>
    void CreateAndSetSceneImpl(Args&&... args) {
        static_assert(std::is_base_of_v<CAbstractScene, GameSceneType>,
                      "GameSceneType must derive from CAbstractScene");

        auto scene = std::make_unique<GameSceneType>(
            mSystem, std::forward<Args>(args)...);
        SetCurrentScene(std::move(scene));
    }
    void SetCurrentScene(std::unique_ptr<Scene::CAbstractScene>&& scene);

    Core::CEngineLoop& mEngineLoop;
    const System::CSystem& mSystem;
};
} // namespace Scene
