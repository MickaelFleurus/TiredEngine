#pragma once

#include "engine/scene/AbstractScene.h"

namespace Scene {
class CDebugScene : public CAbstractScene {
public:
    explicit CDebugScene(Component::CComponentManager& componentManager,
                         const Core::CWindowData& windowData);

    void Update(float deltaTime) override;
    CAbstractScene* GetNextScene() const override;
    const char* GetName() const override;
};
} // namespace Scene