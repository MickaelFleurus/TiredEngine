#pragma once

#include "engine/scene/AbstractScene.h"

namespace Scene {
class DebugScene : public AbstractScene {
public:
    explicit DebugScene(Component::CComponentManager& componentManager,
                        const Core::CWindowData& windowData);

    void Update(float deltaTime) override;
    AbstractScene* GetNextScene() const override;
};
} // namespace Scene