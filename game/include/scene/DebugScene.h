#pragma once

#include <functional>

#include "engine/scene/AbstractScene.h"

namespace Scene {
class CDebugScene : public CAbstractScene {
public:
    explicit CDebugScene(Component::CComponentManager& componentManager,
                         Font::CFontHandler& fontHandler,
                         Core::CMeshManager& meshManager,
                         const System::CSystem& system);

    void Update(float deltaTime) override;
    CAbstractScene* GetNextScene() const override;
    const char* GetName() const override;

    void Load() override;
    void Unload() override;

private:
    std::function<void(CAbstractScene&)> mLoadCallback;
};
} // namespace Scene