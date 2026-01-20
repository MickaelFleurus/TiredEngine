#pragma once
#include "engine/core/GameObjectBuilder.h"

namespace Component {
class CComponentManager;
}

namespace System {
class CSystem;
}

namespace Core {
class CMeshManager;
class CGameObjectManager;
} // namespace Core

namespace Font {
class CFontHandler;
}

namespace Scene {
class CAbstractScene {
public:
    explicit CAbstractScene(Component::CComponentManager& componentManager,
                            Font::CFontHandler& fontHandler,
                            Core::CMeshManager& meshManager,
                            const System::CSystem& system,
                            Core::CGameObjectManager& gameObjectManager);
    virtual ~CAbstractScene();
    virtual void Update(float deltaTime) = 0;
    virtual CAbstractScene* GetNextScene() const = 0;
    virtual void Load() = 0;
    virtual void Unload() = 0;

    virtual const char* GetName() const = 0;
    Core::CGameObjectBuilder& GetObjectBuilder();

protected:
    Core::CGameObjectBuilder mGameObjectBuilder;
};
} // namespace Scene
