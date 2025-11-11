function(setup_msdfgen)
    FetchContent_Declare(
        msdfgen
        GIT_REPOSITORY https://github.com/Chlumsky/msdfgen.git
        GIT_TAG v1.12.1
        SOURCE_DIR external/msdfgen
    ) 
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static" FORCE)
       # Configure msdfgen options - DON'T use CORE_ONLY since msdf-atlas-gen needs extensions
    set(MSDFGEN_CORE_ONLY OFF CACHE BOOL "" FORCE)
    set(MSDFGEN_BUILD_STANDALONE OFF CACHE BOOL "" FORCE)
    set(MSDFGEN_USE_VCPKG OFF CACHE BOOL "" FORCE)
    set(MSDFGEN_USE_OPENMP OFF CACHE BOOL "" FORCE)
    set(MSDFGEN_USE_CPP11 ON CACHE BOOL "" FORCE)
    set(MSDFGEN_INSTALL OFF CACHE BOOL "" FORCE)
    set(MSDFGEN_USE_SKIA OFF CACHE BOOL "" FORCE)
    set(MSDFGEN_DISABLE_SVG ON CACHE BOOL "" FORCE)  # Disable SVG to avoid tinyxml2 dependency
    set(MSDFGEN_DISABLE_PNG ON CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(msdfgen)
endfunction()
