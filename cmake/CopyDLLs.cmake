function(copy_dll_dependencies target)
    if(WIN32)        
        # Create a more robust script
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target}>"
            # Copy SDL3 DLL
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/_deps/sdl3-build/$<CONFIG>/SDL3.dll"
            "$<TARGET_FILE_DIR:${target}>/"
            COMMAND_EXPAND_LISTS
        )
    endif()
endfunction()
