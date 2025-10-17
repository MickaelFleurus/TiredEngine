#pragma once
#include "component/IComponent.h"
#include "input/InputCallback.h"
#include "input/InputWatcher.h"

#include <functional>
#include <optional>

namespace Component {

class CInputComponent : public IComponent {
public:
    explicit CInputComponent(Core::CGameObject& owner, CComponentManager& componentManager,
                             std::optional<Input::InputFunc> onFirePressed,
                             std::optional<Input::InputFunc> onLeftPressed,
                             std::optional<Input::InputFunc> onRightPressed);

    void update(float deltaTime) override;

private:
    const Input::CInputWatcher mInputWatcher;
    std::optional<Input::InputFunc> mOnFirePressed;
    std::optional<Input::InputFunc> mOnLeftPressed;
    std::optional<Input::InputFunc> mOnRightPressed;
};
} // namespace Component