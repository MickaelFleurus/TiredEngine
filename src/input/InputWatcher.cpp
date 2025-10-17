#include "input/InputWatcher.h"

namespace Input {

std::unordered_map<EButton, bool> CInputWatcher::mPreviousButtonStates;
std::unordered_map<EButton, bool> CInputWatcher::mButtonStates;

EButtonState CInputWatcher::getButtonState(EButton button) const {
    if (isPressed(button)) {
        return EButtonState::Pressed;
    } else if (isHolding(button)) {
        return EButtonState::Held;
    } else if (isReleased(button)) {
        return EButtonState::Released;
    } else {
        return EButtonState::None;
    }
}

bool CInputWatcher::isPressed(EButton button) const {
    return !mPreviousButtonStates.at(button) && mButtonStates.at(button);
}

bool CInputWatcher::isHolding(EButton button) const {
    return mPreviousButtonStates.at(button) && mButtonStates.at(button);
}

bool CInputWatcher::isReleased(EButton button) const {
    return mPreviousButtonStates.at(button) && !mButtonStates.at(button);
}

void CInputWatcher::swap() {
    std::swap(mPreviousButtonStates, mButtonStates);
}

} // namespace Input