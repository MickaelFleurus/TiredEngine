#pragma once
#include "core/Inputs.h"
#include "debug/Overlord.h"
#include "renderer/ShaderFactory.h"
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
    Renderer::CShaderFactory m_ShaderFactory;
    Debug::COverlord m_Overlord;
    CInputs m_Inputs;
};
} // namespace Core