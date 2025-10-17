#pragma once
#include "input/InputHandler.h"

namespace Debug {
class COverlord;
}

namespace Core {
class CInputs {
public:
    CInputs(Debug::COverlord& overlord);
    ~CInputs();

    bool Poll();

private:
    Debug::COverlord& m_Overlord;
    Input::CInputHandler m_InputHandler;
};
} // namespace Core