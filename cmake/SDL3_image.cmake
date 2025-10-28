function(setup_sdl3_image)
    FetchContent_Declare(
        SDL3_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-3.2.4
        SOURCE_DIR external/SDL3_image
    )
    FetchContent_MakeAvailable(SDL3_image)

endfunction()