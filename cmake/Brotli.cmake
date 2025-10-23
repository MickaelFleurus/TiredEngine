function(setup_brotli)
    FetchContent_Declare(
        brotli
        GIT_REPOSITORY https://github.com/google/brotli.git
        GIT_TAG v1.1.0
        SOURCE_DIR external/brotli
    )
    set(BROTLI_DISABLE_TESTS ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(brotli)
endfunction()