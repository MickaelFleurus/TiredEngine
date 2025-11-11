
function(setup_glm)
    set(GLM_BUILD_LIBRARY OFF CACHE BOOL "" FORCE)
    set(GLM_ENABLE_CXX_20 ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.2
        SOURCE_DIR external/glm
    )
    FetchContent_MakeAvailable(glm)

endfunction()