function(setup_harfbuzz)
   FetchContent_Declare(
        HarfBuzz
        GIT_REPOSITORY https://github.com/harfbuzz/harfbuzz.git
        GIT_TAG 2.3.1
        SOURCE_DIR external/harfbuzz
    )
    set(HB_HAVE_FREETYPE ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(HarfBuzz)

    # Tell SDL_ttf where to find FreeType
    set(FREETYPE_LIBRARY freetype CACHE STRING "" FORCE)
    set(FREETYPE_INCLUDE_DIRS ${freetype_SOURCE_DIR}/include CACHE STRING "" FORCE)
    
    # Also set the uppercase versions (some packages check these)
    set(HARFBUZZ_FOUND TRUE CACHE BOOL "" FORCE)
    set(HARFBUZZ_VERSION "12.1.0" CACHE STRING "" FORCE)
    set(HARFBUZZ_LIBRARY harfbuzz CACHE STRING "" FORCE)
    set(HARFBUZZ_LIBRARIES harfbuzz CACHE STRING "" FORCE)
    set(HARFBUZZ_INCLUDE_DIRS ${harfbuzz_SOURCE_DIR}/src CACHE STRING "" FORCE)
endfunction()