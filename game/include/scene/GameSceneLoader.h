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
}

namespace System {
class CSystem;
}

namespace Scene {

class CGameSceneLoader : public CSceneLoader {
public:
    CGameSceneLoader(Core::CEngineLoop& engineLoop,
                     Utils::CFileHandler& fileHandler,
                     Renderer::CTextureManager& textureManager,
                     Renderer::CSpriteManager& spriteManager,
                     Component::CComponentManager& componentManager,
                     Font::CFontHandler& fontHandler,
                     Core::CMeshManager& meshManager,
                     const System::CSystem& system);
    ~CGameSceneLoader() override;

    std::unique_ptr<Scene::CAbstractScene>
    LoadGameSpecificScenePart(const YAML::Node& sceneData) override;

private:
    Component::CComponentManager& mComponentManager;
    Font::CFontHandler& mFontHandler;
    Core::CMeshManager& mMeshManager;
    const System::CSystem& mSystem;
};
} // namespace Scene
