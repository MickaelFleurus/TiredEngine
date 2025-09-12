#include "core/GameLoop.h"

#include <SDL3/SDL.h>

namespace Core {

CGameLoop::CGameLoop() = default;

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
        m_Window.Render();
    }
    return false;
}
} // namespace Core