#pragma once

#include "engine/core/GameObjectId.h"

namespace Renderer {
class IDirtyObserver {
public:
    virtual ~IDirtyObserver() = default;

    virtual void OnDirty(Core::GameObjectId id) = 0;
};
} // namespace Renderer
