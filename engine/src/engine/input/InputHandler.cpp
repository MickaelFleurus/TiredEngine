#include "engine/input/InputHandler.h"
#include "engine/input/InputTypes.h"
#include <SDL3/SDL_events.h>
#include <optional>

namespace {
constexpr auto buttonList = {Input::EButton::Left, Input::EButton::Right,
                             Input::EButton::Fire};

SDL_Scancode GetButtonKey(Input::EButton button) {
    switch (button) {
    case Input::EButton::Fire:
        return SDL_GetScancodeFromKey(SDLK_SPACE, nullptr);
    case Input::EButton::Left:
        return SDL_GetScancodeFromKey(SDLK_LEFT, nullptr);
    case Input::EButton::Right:
        return SDL_GetScancodeFromKey(SDLK_RIGHT, nullptr);
    }
}
} // namespace

namespace Input {

CInputHandler::CInputHandler() {
    for (const auto button : buttonList) {
        mInputStateWatcher.mButtonStates[button] = false;
    }
    mInputStateWatcher.mPreviousButtonStates = mInputStateWatcher.mButtonStates;
}

void CInputHandler::Update() {
    auto keyboardState = SDL_GetKeyboardState(nullptr);
    for (const auto button : buttonList) {
        mInputStateWatcher.mButtonStates[button] =
            keyboardState[GetButtonKey(button)];
    }
}

void CInputHandler::Swap() {
    mInputStateWatcher.swap();
}
} // namespace Input