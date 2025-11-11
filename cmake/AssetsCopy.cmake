function(copy_assets_to_target target)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/engine/assets"
            "$<TARGET_FILE_DIR:${target}>/assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/game/assets"
            "$<TARGET_FILE_DIR:${target}>/assets"
        COMMENT "Copying assets to target directory"
        VERBATIM
    )
endfunction()