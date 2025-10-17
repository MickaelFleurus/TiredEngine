file(GLOB SHADERS "${CMAKE_SOURCE_DIR}/shaders/*.vert" "${CMAKE_SOURCE_DIR}/shaders/*.frag")

# Find Vulkan package to get glslc
find_package(Vulkan REQUIRED)

set(COMPILED_SHADERS "")
foreach(SHADER ${SHADERS})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    
    # Compile to SPIR-V
    set(SPIRV "${CMAKE_BINARY_DIR}/shaders/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/shaders"
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} -o ${SPIRV}
        DEPENDS ${SHADER}
        COMMENT "Compiling ${FILE_NAME} to SPIR-V"
    )
    
    list(APPEND COMPILED_SHADERS ${SPIRV})
    
    # Platform-specific shader compilation
    if(WIN32)
        # Convert to DXIL for Windows (if spirv-cross is available)
        find_program(SPIRV_CROSS spirv-cross)
        if(SPIRV_CROSS)
            set(DXIL "${CMAKE_BINARY_DIR}/shaders/${FILE_NAME}.dxil")
            add_custom_command(
                OUTPUT ${DXIL}
                COMMAND ${SPIRV_CROSS} ${SPIRV} --hlsl --shader-model 50 --output ${DXIL}
                DEPENDS ${SPIRV}
                COMMENT "Converting ${FILE_NAME} to DXIL"
            )
            list(APPEND COMPILED_SHADERS ${DXIL})
        endif()
    endif()
    
    if(APPLE)
        # Convert to MSL for macOS (Metal)
        find_program(SPIRV_CROSS spirv-cross)
        if(SPIRV_CROSS)
            set(MSL "${CMAKE_BINARY_DIR}/shaders/${FILE_NAME}.msl")
            add_custom_command(
                OUTPUT ${MSL}
                COMMAND ${SPIRV_CROSS} ${SPIRV} --msl --output ${MSL}
                DEPENDS ${SPIRV}
                COMMENT "Converting ${FILE_NAME} to MSL"
            )
            list(APPEND COMPILED_SHADERS ${MSL})
        endif()
    endif()
    
endforeach()

add_custom_target(CompileShaders ALL DEPENDS ${COMPILED_SHADERS})

