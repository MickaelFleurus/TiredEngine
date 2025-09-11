#pragma once
#include "core/Inputs.h"
#include "renderer/Window.h"
#include <expected>

namespace Core {
class CGameLoop {
public:
    CGameLoop();
    std::expected<void, const char*> EverythingInitialisedCorrectly() const;

    bool Run();

private:
    Renderer::CWindow m_Window;
    CInputs m_Inputs;
};
} // namespace Core