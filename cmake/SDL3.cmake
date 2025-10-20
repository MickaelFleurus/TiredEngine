function(setup_sdl3)
    set(SDL3_SHARED ON CACHE BOOL "" FORCE)
    set(SDL3_STATIC OFF CACHE BOOL "" FORCE)
    find_package(SDL3 QUIET)
    if(NOT SDL3_FOUND)
        FetchContent_Declare(
            SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-3.2.22
            SOURCE_DIR external/SDL3
        )
        FetchContent_MakeAvailable(SDL3)
    endif()
endfunction()