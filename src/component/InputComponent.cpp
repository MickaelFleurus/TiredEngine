#include "component/InputComponent.h"
#include "core/GameObject.h"
#include "input/InputState.h"
#include "input/InputTypes.h"

namespace Component {
CInputComponent::CInputComponent(Core::CGameObject& owner, CComponentManager& componentManager,
                                 std::optional<Input::InputFunc> onFirePressed,
                                 std::optional<Input::InputFunc> onLeftPressed,
                                 std::optional<Input::InputFunc> onRightPressed)
    : IComponent(owner, componentManager)
    , mOnFirePressed(onFirePressed)
    , mOnLeftPressed(onLeftPressed)
    , mOnRightPressed(onRightPressed) {}

void CInputComponent::update(float /*deltaTime*/) {

    auto fireState = mInputWatcher.getButtonState(Input::EButton::Fire);
    if (fireState != Input::EButtonState::None) {
        if (mOnFirePressed) {
            (*mOnFirePressed)(mComponentManager, mOwner.getId(), fireState);
        }
    }

    auto leftState = mInputWatcher.getButtonState(Input::EButton::Left);
    if (leftState != Input::EButtonState::None) {
        if (mOnLeftPressed) {
            (*mOnLeftPressed)(mComponentManager, mOwner.getId(), leftState);
        }
    }

    auto rightState = mInputWatcher.getButtonState(Input::EButton::Right);
    if (rightState != Input::EButtonState::None) {
        if (mOnRightPressed) {
            (*mOnRightPressed)(mComponentManager, mOwner.getId(), rightState);
        }
    }
}
} // namespace Component