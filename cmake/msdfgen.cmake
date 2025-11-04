function(setup_msdfgen)
    FetchContent_Declare(
        msdfgen
        GIT_REPOSITORY https://github.com/Chlumsky/msdfgen.git
        GIT_TAG v1.12.1
        SOURCE_DIR external/msdfgen
    )
    
    # Configure msdfgen options - use CORE_ONLY to avoid external dependencies
    set(MSDFGEN_CORE_ONLY ON CACHE BOOL "" FORCE)
    set(MSDFGEN_BUILD_STANDALONE OFF CACHE BOOL "" FORCE)
    set(MSDFGEN_USE_VCPKG OFF CACHE BOOL "" FORCE)
    set(MSDFGEN_USE_OPENMP OFF CACHE BOOL "" FORCE)
    set(MSDFGEN_USE_CPP11 ON CACHE BOOL "" FORCE)
    set(MSDFGEN_INSTALL OFF CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(msdfgen)
endfunction()
