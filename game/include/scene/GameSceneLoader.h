#pragma once

#include <engine/scene/SceneLoader.h>

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
                     Core::CEntityManager& entityManager,
                     Component::CManager& componentManager,
                     const System::CSystem& system);

private:
    const System::CSystem& mSystem;
};
} // namespace Scene
