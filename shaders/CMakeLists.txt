file(GLOB SHADERS "${CMAKE_SOURCE_DIR}/shaders/*.vert" "${CMAKE_SOURCE_DIR}/shaders/*.frag")

# Use glslc from Vulkan SDK (already available since you have Vulkan)
set(COMPILED_SHADERS "")
foreach(SHADER ${SHADERS})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    
    # Compile to SPIR-V
    set(SPIRV "${CMAKE_BINARY_DIR}/shaders/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} -o ${SPIRV}
        DEPENDS ${SHADER}
        COMMENT "Compiling ${FILE_NAME} to SPIR-V"
    )
    
    # Convert to DXIL (if spirv-cross is available)
    set(DXIL "${CMAKE_BINARY_DIR}/shaders/${FILE_NAME}.dxil")
    add_custom_command(
        OUTPUT ${DXIL}
        COMMAND spirv-cross ${SPIRV} --hlsl --shader-model 50 --output ${DXIL}
        DEPENDS ${SPIRV}
        COMMENT "Converting ${FILE_NAME} to DXIL"
    )
    
    # Convert to MSL (for Metal)
    set(MSL "${CMAKE_BINARY_DIR}/shaders/${FILE_NAME}.msl")
    add_custom_command(
        OUTPUT ${MSL}
        COMMAND spirv-cross ${SPIRV} --msl --output ${MSL}
        DEPENDS ${SPIRV}
        COMMENT "Converting ${FILE_NAME} to MSL"
    )
    
    list(APPEND COMPILED_SHADERS ${SPIRV} ${DXIL} ${MSL})
endforeach()

add_custom_target(CompileShaders ALL DEPENDS ${COMPILED_SHADERS})

