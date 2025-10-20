#pragma once

#include "engine/scene/AbstractScene.h"

namespace Scene {
class DebugScene : public AbstractScene {
public:
    explicit DebugScene(Component::CComponentManager& componentManager,
                        const Core::SWindowData& windowData);
};
} // namespace Scene