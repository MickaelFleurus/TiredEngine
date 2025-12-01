#pragma once
#include "engine/scene/ISceneHandler.h"

#include <memory>
#include <span>

namespace System {
class CSystem;
}

namespace Component {
class CComponentManager;
}

namespace Renderer {
class CMeshRenderer;
}

namespace Core {
class CWindowData;
class CEngineLoop;
class CMeshManager;
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
        Font::CFontHandler& fontHandler, Core::CMeshManager& meshManager,
        const System::CSystem& system);

    CAbstractScene* GetCurrentScene() const override;

protected:
    template <typename GameSceneType, typename... Args>
    void CreateAndSetSceneImpl(Args&&... args) {
        static_assert(std::is_base_of_v<CAbstractScene, GameSceneType>,
                      "GameSceneType must derive from CAbstractScene");

        auto scene = std::make_unique<GameSceneType>(
            mComponentManager, mFontHandler, mMeshManager, mSystem,
            std::forward<Args>(args)...);
        SetCurrentScene(std::move(scene));
    }
    void SetCurrentScene(std::unique_ptr<Scene::CAbstractScene>&& scene);

    Core::CEngineLoop& mEngineLoop;
    Component::CComponentManager& mComponentManager;
    Font::CFontHandler& mFontHandler;
    Core::CMeshManager& mMeshManager;
    const System::CSystem& mSystem;
};
} // namespace Scene