#include "engine/core/Inputs.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>

#include "engine/debug/OverlordManager.h"
#include "engine/renderer/Window.h"

namespace Core {

CInputs::CInputs(Debug::COverlordManager& overlord)
    : mOverlordManager(overlord) {
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
        mOverlordManager.HandleEvent(&e);
    }
    return true;
}

} // namespace Core