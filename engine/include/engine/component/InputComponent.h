// #pragma once
// #include <functional>
// #include <optional>

// #include "engine/component/IComponent.h"
// #include "engine/input/InputCallback.h"
// #include "engine/input/InputWatcher.h"

// namespace Component {

// class CInputComponent : public IComponent {
// public:
//     explicit CInputComponent(Core::GameObjectId& owner,
//                              CComponentManager& componentManager,
//                              std::optional<Input::InputFunc> onFirePressed,
//                              std::optional<Input::InputFunc> onLeftPressed,
//                              std::optional<Input::InputFunc> onRightPressed);
//     CInputComponent(Core::GameObjectId& owner,
//                     CComponentManager& componentManager,
//                     const CInputComponent& other);
//     CInputComponent(const CInputComponent& other) = delete;
//     CInputComponent(const CInputComponent&& other) = delete;
//     CInputComponent& operator=(CInputComponent&& other) = delete;

//     CInputComponent& operator=(const CInputComponent& other);

//     void Update(float deltaTime) override;

// private:
//     const Input::CInputWatcher mInputWatcher;
//     // std::optional<Input::InputFunc> mOnFirePressed;
//     // std::optional<Input::InputFunc> mOnLeftPressed;
//     // std::optional<Input::InputFunc> mOnRightPressed;
// };
// } // namespace Component
