#include "core/GameLoop.h"

#include <SDL3/SDL.h>

namespace Core {

CGameLoop::CGameLoop()
    : m_Overlord(m_Window.Get(), m_Window.GetDevice())
    , m_ShaderFactory(m_Window.GetDevice())
    , m_Inputs(m_Overlord) {
}

std::expected<void, const char*>
CGameLoop::EverythingInitialisedCorrectly() const {
    const char* error = SDL_GetError();
    if (error && *error) {
        return std::unexpected(error);
    }
    return {};
}

bool CGameLoop::Run() {
    while (m_Inputs.Poll()) {
        if (!m_Window.PrepareRender()) {
            SDL_Log("Failed to prepare render");
            continue;
        }
        m_Overlord.PrepareRender(m_Window.GetCommandBuffer());
        if (m_Window.BeginRender()) {
            m_Overlord.Render(m_Window.GetCommandBuffer(),
                              m_Window.GetRenderPass());
            m_Window.Render();
            m_Window.EndRender();
        }
    }
    return false;
}
} // namespace Core