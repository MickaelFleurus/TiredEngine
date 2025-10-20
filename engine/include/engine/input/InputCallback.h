#pragma once

#include "engine/core/GameObjectId.h"
#include "engine/input/InputState.h"
#include <functional>

namespace Component {
class CComponentManager;
}

namespace Input {
using InputFunc = std::function<void(Component::CComponentManager&,
                                     Core::GameObjectId, EButtonState)>;
}