function(setup_freetype)
   FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
        GIT_TAG VER-2-14-1
        SOURCE_DIR external/freetype
    )
    set(FT_DISABLE_ZLIB OFF CACHE BOOL "" FORCE)
    set(FT_DISABLE_PNG OFF CACHE BOOL "" FORCE)
    set(FT_DISABLE_HARFBUZZ OFF CACHE BOOL "" FORCE)
    set(FT_DISABLE_BROTLI OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(freetype)
endfunction()