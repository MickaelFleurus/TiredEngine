function(setup_zlib)
    # Configure zlib build options
    set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "Build zlib examples" FORCE)
    
    FetchContent_Declare(
        zlib
        GIT_REPOSITORY https://github.com/madler/zlib.git
        GIT_TAG v1.3.1
        SOURCE_DIR external/zlib
    )
    
    FetchContent_MakeAvailable(zlib)
    
    # Create consistent target names for both Debug and Release
    if(TARGET zlibstatic)
        # Make sure the target is available under both names 
        add_library(ZLIB::ZLIB ALIAS zlibstatic)
        add_library(zlib::zlib ALIAS zlibstatic)
    endif()
    set(ZLIB_LIBRARY zlibstatic CACHE STRING "" FORCE)
    set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} CACHE STRING "" FORCE)
endfunction()