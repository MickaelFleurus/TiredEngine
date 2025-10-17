#pragma once

#include "input/InputWatcher.h"

union SDL_Event;

namespace Input {
class CInputHandler {
public:
    CInputHandler();
    void HandleEvent(SDL_Event* event);
    void Update();
    void Swap();

private:
    CInputWatcher mInputStateWatcher;
};
} // namespace Input