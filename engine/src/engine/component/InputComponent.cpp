#include "engine/component/InputComponent.h"

#include "engine/core/GameObject.h"
#include "engine/input/InputState.h"
#include "engine/input/InputTypes.h"

namespace Component {
CInputComponent::CInputComponent(Core::CGameObject& owner,
                                 CComponentManager& componentManager,
                                 std::optional<Input::InputFunc> onFirePressed,
                                 std::optional<Input::InputFunc> onLeftPressed,
                                 std::optional<Input::InputFunc> onRightPressed)
    : IComponent(owner, componentManager, Core::EDirtyType::None)
    , mOnFirePressed(onFirePressed)
    , mOnLeftPressed(onLeftPressed)
    , mOnRightPressed(onRightPressed) {
}

void CInputComponent::Update(float /*deltaTime*/) {

    auto fireState = mInputWatcher.getButtonState(Input::EButton::Fire);
    if (fireState != Input::EButtonState::None) {
        if (mOnFirePressed) {
            (*mOnFirePressed)(mComponentManager, mOwner.GetId(), fireState);
        }
    }

    auto leftState = mInputWatcher.getButtonState(Input::EButton::Left);
    if (leftState != Input::EButtonState::None) {
        if (mOnLeftPressed) {
            (*mOnLeftPressed)(mComponentManager, mOwner.GetId(), leftState);
        }
    }

    auto rightState = mInputWatcher.getButtonState(Input::EButton::Right);
    if (rightState != Input::EButtonState::None) {
        if (mOnRightPressed) {
            (*mOnRightPressed)(mComponentManager, mOwner.GetId(), rightState);
        }
    }
}
} // namespace Component