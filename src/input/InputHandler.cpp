#include "input/InputHandler.h"
#include "input/InputTypes.h"
#include <SDL3/SDL_events.h>
#include <optional>

namespace {
constexpr auto buttonList = {Input::EButton::Left, Input::EButton::Right,
                             Input::EButton::Fire};

std::optional<Input::EButton> GetButtonId(SDL_Keycode key) {
    switch (key) {
    case SDLK_SPACE:
        return Input::EButton::Fire;
    case SDLK_LEFT:
        return Input::EButton::Left;
    case SDLK_RIGHT:
        return Input::EButton::Right;
    }
    return std::nullopt;
}

namespace Input {

CInputHandler::CInputHandler() {
    for (const auto button : buttonList) {
        mInputStateWatcher.mButtonStates[button] = false;
    }
}

void CInputHandler::HandleEvent(SDL_Event* event) {
    if (auto button = GetButtonId(event->key.key); button.has_value()) {
        bool isPressed = (event->type == SDL_EVENT_KEY_DOWN);
        mInputStateWatcher.mButtonStates[button.value()] = isPressed;
    }
}

void CInputHandler::Update() {
    mInputStateWatcher.swap();
}
} // namespace Input