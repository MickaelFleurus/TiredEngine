function(setup_spirv_reflect)
    FetchContent_Declare(
        SPIRV-Reflect
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Reflect.git
        GIT_TAG vulkan-sdk-1.4.328.1
        SOURCE_DIR external/SPIRV-Reflect
    )
    set(SPIRV_REFLECT_EXECUTABLE OFF CACHE BOOL "" FORCE)
    set(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SPIRV-Reflect)

endfunction()