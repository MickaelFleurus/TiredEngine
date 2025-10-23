#pragma once

#include "engine/scene/AbstractScene.h"

namespace Scene {
class DebugScene : public CAbstractScene {
public:
    explicit DebugScene(Component::CComponentManager& componentManager,
                        const Core::CWindowData& windowData);

    void Update(float deltaTime) override;
    CAbstractScene* GetNextScene() const override;
    const char* GetName() const override;
};
} // namespace Scene