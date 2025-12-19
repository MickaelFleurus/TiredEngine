#pragma once

#include "engine/scene/AbstractScene.h"

namespace Core {
class CGrid;
} // namespace Core

namespace Scene {
class CGameplayScene : public CAbstractScene {
public:
    explicit CGameplayScene(Component::CComponentManager& componentManager,
                            Font::CFontHandler& fontHandler,
                            Core::CMeshManager& meshManager,
                            const System::CSystem& system);
    ~CGameplayScene() override;

    void Update(float deltaTime) override;
    CAbstractScene* GetNextScene() const override;
    const char* GetName() const override;

    void Load() override;
    void Unload() override;

    Core::CGrid& CreateGrid(int width, int height);

private:
    std::unique_ptr<Core::CGrid> mGrid;
};
} // namespace Scene