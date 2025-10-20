#pragma once

#include "engine/input/InputState.h"
#include "engine/input/InputTypes.h"

#include <unordered_map>

namespace Input {
class CInputWatcher {
public:
    CInputWatcher() = default;

    EButtonState getButtonState(EButton button) const;
    bool isPressed(EButton button) const;
    bool isHolding(EButton button) const;
    bool isReleased(EButton button) const;

private:
    void swap();
    static std::unordered_map<EButton, bool> mPreviousButtonStates;
    static std::unordered_map<EButton, bool> mButtonStates;

    friend class CInputHandler;
};
} // namespace Input
