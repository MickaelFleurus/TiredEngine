#pragma once

#include "core/GameObjectId.h"
#include "input/InputState.h"
#include <functional>

namespace Component {
class CComponentManager;
}

namespace Input {
using InputFunc =
    std::function<void(Component::CComponentManager&, Core::GameObjectId, EButtonState)>;
}