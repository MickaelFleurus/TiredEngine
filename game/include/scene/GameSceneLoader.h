#pragma once

#include <engine/scene/SceneLoader.h>

namespace Component {
class CComponentManager;
}

namespace Font {
class CFontHandler;
}

namespace Core {
class CMeshManager;
class CGameObjectManager;
} // namespace Core

namespace System {
class CSystem;
}

namespace Scene {

class CGameSceneLoader : public CSceneLoader {
public:
    CGameSceneLoader(Core::CEngineLoop& engineLoop,
                     Utils::CFileHandler& fileHandler,
                     const System::CSystem& system);
    ~CGameSceneLoader() override;

    std::unique_ptr<Scene::CAbstractScene>
    LoadGameSpecificScenePart(const YAML::Node& sceneData) override;

private:
    const System::CSystem& mSystem;
};
} // namespace Scene
