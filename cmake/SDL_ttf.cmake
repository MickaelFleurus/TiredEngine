function(setup_sdlttf)
    # Use SDL3_ttf's vendored dependencies instead of trying to provide them
    set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
    set(SDLTTF_INSTALL OFF CACHE BOOL "" FORCE)
    set(SDLTTF_BUILD_SHARED ON CACHE BOOL "" FORCE)
    
    FetchContent_Declare(
        sdl3_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG release-3.2.2
        SOURCE_DIR external/SDL_ttf
    )
    FetchContent_MakeAvailable(sdl3_ttf)
    
endfunction()