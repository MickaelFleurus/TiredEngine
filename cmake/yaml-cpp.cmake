function(setup_yaml_cpp)
    FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG 0.8.0
        SOURCE_DIR external/yaml-cpp
    )
    set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)

    set(CMAKE_POLICY_VERSION_MINIMUM 3.15)
    FetchContent_MakeAvailable(yaml-cpp)
endfunction()
