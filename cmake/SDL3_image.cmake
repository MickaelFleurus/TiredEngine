function(setup_sdl3_image)    

    set(BUILD_SHARED_LIBS ON CACHE BOOL "Build dynamic" FORCE)
    # Disable all optional formats that might cause issues
    set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_JXL OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_VENDORED OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    
    # Explicitly disable dav1d
    set(BUILD_DAV1D OFF CACHE BOOL "" FORCE)
    set(AVIF_CODEC_DAV1D OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        SDL3_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-3.2.4
        SOURCE_DIR external/SDL3_image
    )
    FetchContent_MakeAvailable(SDL3_image)
endfunction()