function(setup_pnglib)
     FetchContent_Declare(
        libpng
        GIT_REPOSITORY https://github.com/glennrp/libpng.git
        GIT_TAG v1.6.50
        SOURCE_DIR external/libpng
    )
    set(PNG_SHARED OFF CACHE BOOL "" FORCE)
    set(PNG_STATIC ON CACHE BOOL "" FORCE)
    set(PNG_TESTS OFF CACHE BOOL "" FORCE)
    # Fix for the install(EXPORT libpng) error
    # Ensure libpng creates the export by not skipping the install targets
    set(SKIP_INSTALL_LIBRARIES OFF CACHE BOOL "" FORCE)
    set(SKIP_INSTALL_ALL OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(libpng)
endfunction()