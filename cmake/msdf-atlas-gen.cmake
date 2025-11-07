function(setup_msdfatlasgen)
    FetchContent_Declare(
        msdfatlasgen
        GIT_REPOSITORY https://github.com/Chlumsky/msdf-atlas-gen.git
        GIT_TAG v1.3
        SOURCE_DIR external/msdfatlasgen
    )
    
    # Configure msdfgen options - use CORE_ONLY to avoid external dependencies
    set(MSDF_ATLAS_BUILD_STANDALONE OFF CACHE BOOL "" FORCE)
    set(MSDF_ATLAS_USE_VCPKG OFF CACHE BOOL "" FORCE)
    set(MSDF_ATLAS_NO_ARTERY_FONT OFF CACHE BOOL "" FORCE)
    set(MSDF_ATLAS_MSDFGEN_EXTERNAL ON CACHE BOOL "" FORCE)   
    FetchContent_MakeAvailable(msdfatlasgen)
endfunction()
