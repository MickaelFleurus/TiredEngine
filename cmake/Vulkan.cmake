function(setup_vulkan)
    # Find Vulkan package
    find_package(Vulkan REQUIRED COMPONENTS glslc)
    
    if(NOT Vulkan_FOUND)
        message(FATAL_ERROR "Vulkan SDK not found. Please install the Vulkan SDK from https://vulkan.lunarg.com/")
    endif()
    
    message(STATUS "Vulkan found: ${Vulkan_INCLUDE_DIRS}")
    message(STATUS "Vulkan library: ${Vulkan_LIBRARIES}")
    
    if(Vulkan_glslc_FOUND)
        message(STATUS "glslc found: ${Vulkan_GLSLC_EXECUTABLE}")
    endif()
    
    # On macOS, ensure MoltenVK is found
    if(APPLE)
        if(NOT Vulkan_LIBRARY)
            message(FATAL_ERROR "MoltenVK not found. Please install Vulkan SDK for macOS from https://vulkan.lunarg.com/sdk/home#mac")
        endif()
        message(STATUS "MoltenVK will be used on macOS")
        
        # MoltenVK requires specific frameworks on macOS
        find_library(METAL_FRAMEWORK Metal)
        find_library(FOUNDATION_FRAMEWORK Foundation)
        find_library(QUARTZCORE_FRAMEWORK QuartzCore)
        find_library(IOSURFACE_FRAMEWORK IOSurface)
        
        if(TARGET Vulkan::Vulkan)
            target_link_libraries(Vulkan::Vulkan INTERFACE
                ${METAL_FRAMEWORK}
                ${FOUNDATION_FRAMEWORK}
                ${QUARTZCORE_FRAMEWORK}
                ${IOSURFACE_FRAMEWORK}
            )
        endif()
    endif()
endfunction()