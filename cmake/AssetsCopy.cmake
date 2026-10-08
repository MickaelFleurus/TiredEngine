function(copy_assets_to_target target)
    add_custom_command(TARGET ${target} PRE_BUILD
        COMMAND "${CMAKE_COMMAND}"
            "-DSOURCE_DIR=${CMAKE_SOURCE_DIR}/engine/assets"
            "-DDESTINATION_DIR=$<TARGET_FILE_DIR:${target}>/assets"
            "-DEXCLUDED_RELATIVE_DIR=sprites"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/CopyDirectoryExcept.cmake"
        COMMAND "${CMAKE_COMMAND}"
            "-DSOURCE_DIR=${CMAKE_SOURCE_DIR}/game/assets"
            "-DDESTINATION_DIR=$<TARGET_FILE_DIR:${target}>/assets"
            "-DEXCLUDED_RELATIVE_DIR=sprites"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/CopyDirectoryExcept.cmake"
        COMMENT "Copying assets to target directory"
        VERBATIM
    )
endfunction()
