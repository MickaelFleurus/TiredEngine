# Function to compile shaders and place them in the target's directory
function(compile_shaders_to_target target)
    # Find Vulkan package to get glslc
    find_package(Vulkan REQUIRED)
    
    # Collect all shaders from both engine and game
    file(GLOB ENGINE_SHADERS 
        "${CMAKE_SOURCE_DIR}/engine/assets/shaders/*.vert" 
        "${CMAKE_SOURCE_DIR}/engine/assets/shaders/*.frag"
        "${CMAKE_SOURCE_DIR}/engine/assets/shaders/*.vertex"
    )
    
    file(GLOB GAME_SHADERS 
        "${CMAKE_SOURCE_DIR}/game/assets/shaders/*.vert" 
        "${CMAKE_SOURCE_DIR}/game/assets/shaders/*.frag"
        "${CMAKE_SOURCE_DIR}/game/assets/shaders/*.vertex"
    )
    
    set(ALL_SHADERS ${ENGINE_SHADERS} ${GAME_SHADERS})
    
    if(NOT ALL_SHADERS)
        message(STATUS "No shaders found to compile for ${target}")
        return()
    endif()

    set(COMPILED_SHADERS "")
    
    foreach(SHADER ${ALL_SHADERS})
        get_filename_component(FILE_NAME ${SHADER} NAME)
        
        # Output SPIR-V to target's shader directory
        set(SPIRV_OUTPUT "$<TARGET_FILE_DIR:${target}>/assets/shaders/${FILE_NAME}.spv")
        
        add_custom_command(
            TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target}>/assets/shaders"
            COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} -o ${SPIRV_OUTPUT}
            COMMENT "Compiling shader ${FILE_NAME} to SPIR-V"
            VERBATIM
        )
        
        list(APPEND COMPILED_SHADERS ${SPIRV_OUTPUT})
        
        # Platform-specific shader compilation
        if(WIN32)
            # Convert to DXIL for Windows (if spirv-cross is available)
            find_program(SPIRV_CROSS spirv-cross)
            if(SPIRV_CROSS)
                set(DXIL_OUTPUT "$<TARGET_FILE_DIR:${target}>/assets/shaders/${FILE_NAME}.dxil")
                add_custom_command(
                    TARGET ${target} POST_BUILD
                    COMMAND ${SPIRV_CROSS} ${SPIRV_OUTPUT} --hlsl --shader-model 50 --output ${DXIL_OUTPUT}
                    COMMENT "Converting ${FILE_NAME} to DXIL"
                    VERBATIM
                )
            endif()
        endif()
        
        if(APPLE)
            # Convert to MSL for macOS (Metal)
            find_program(SPIRV_CROSS spirv-cross)
            if(SPIRV_CROSS)
                set(MSL_OUTPUT "$<TARGET_FILE_DIR:${target}>/assets/shaders/${FILE_NAME}.msl")
                add_custom_command(
                    TARGET ${target} POST_BUILD
                    COMMAND ${SPIRV_CROSS} ${SPIRV_OUTPUT} --msl --output ${MSL_OUTPUT}
                    COMMENT "Converting ${FILE_NAME} to MSL"
                    VERBATIM
                )
            endif()
        endif()
        
    endforeach()
    
    message(STATUS "Shader compilation configured for ${target}: ${ALL_SHADERS}")
endfunction()