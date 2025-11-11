function(setup_sdl3)
   
    find_package(SDL3 QUIET)
    if(NOT SDL3_FOUND)
        set(BUILD_SHARED_LIBS ON CACHE BOOL "Build dynamic" FORCE)
        FetchContent_Declare(
            SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-3.2.22
            SOURCE_DIR external/SDL3
        ) 
        set(SDL3_SHARED ON CACHE BOOL "" FORCE)
        set(SDL3_STATIC OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(SDL3)
    endif()
endfunction()