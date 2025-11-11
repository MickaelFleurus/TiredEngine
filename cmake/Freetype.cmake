function(setup_freetype)
    # Disable external dependency finding
    set(FT_DISABLE_ZLIB OFF CACHE BOOL "Enable zlib support" FORCE)
    set(FT_DISABLE_BZIP2 ON CACHE BOOL "Disable bzip2 support" FORCE)
    set(FT_DISABLE_PNG OFF CACHE BOOL "Enable PNG support" FORCE)
    set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "Disable HarfBuzz support" FORCE)
    set(FT_DISABLE_BROTLI OFF CACHE BOOL "Enable Brotli support" FORCE)
    
    # Disable RC compilation by temporarily clearing the RC compiler
    if(DEFINED CMAKE_RC_COMPILER)
        set(CMAKE_RC_COMPILER_BACKUP ${CMAKE_RC_COMPILER})
    else()
        set(CMAKE_RC_COMPILER_BACKUP "")
    endif()
    set(CMAKE_RC_COMPILER "" CACHE STRING "Disable RC compiler" FORCE)
    
    FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://github.com/freetype/freetype.git
        GIT_TAG VER-2-14-1
        SOURCE_DIR external/freetype
    )
    FetchContent_MakeAvailable(freetype)   
    # Restore RC compiler for other targets (only if it was set)
    if(NOT "${CMAKE_RC_COMPILER_BACKUP}" STREQUAL "")
        set(CMAKE_RC_COMPILER ${CMAKE_RC_COMPILER_BACKUP} CACHE STRING "Restore RC compiler" FORCE)
    endif()
    
    # Create Freetype::Freetype alias if it doesn't exist
    if(TARGET freetype AND NOT TARGET Freetype::Freetype)
        add_library(Freetype::Freetype ALIAS freetype)
    endif()

    if(TARGET freetype AND TARGET zlibstatic)
        target_link_libraries(freetype PRIVATE zlibstatic)
    endif()
endfunction()