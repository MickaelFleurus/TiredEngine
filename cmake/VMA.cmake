function(setup_vma)
    FetchContent_Declare(
        VulkanMemoryAllocator
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG v3.4.0
        SOURCE_DIR external/VulkanMemoryAllocator
        SYSTEM
    )
    FetchContent_MakeAvailable(VulkanMemoryAllocator)

endfunction()
