#include "core/Inputs.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_init.h"

#include "debug/Overlord.h"
#include "renderer/Window.h"

namespace Core {

CInputs::CInputs(Debug::COverlord& overlord) : m_Overlord(overlord) {
    SDL_InitSubSystem(SDL_INIT_EVENTS);
}

CInputs::~CInputs() {
    SDL_QuitSubSystem(SDL_INIT_EVENTS);
}

bool CInputs::Poll() {

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            return false;
        }
        m_InputHandler.HandleEvent(&e);
    }
    m_Overlord.HandleEvent(&e);
    return true;
}

} // namespace Core