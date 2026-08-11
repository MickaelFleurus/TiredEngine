
function(setup_fastgltf)
    set(FASTGLTF_COMPILE_AS_CPP20 ON CACHE BOOL "" FORCE)
    set(GLM_ENABLE_CXX_20 ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
        fastgltf
        GIT_REPOSITORY https://github.com/spnda/fastgltf
        GIT_TAG v0.9.0
        SOURCE_DIR external/fastgltf
    )
    FetchContent_MakeAvailable(fastgltf)

endfunction()
