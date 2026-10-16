function(copy_assets_to_target target)
    add_custom_target(${target}_CopyAssets
        COMMAND "${CMAKE_COMMAND}"
            -E rm -f "$<TARGET_FILE_DIR:${target}>/assets/.asset_sources"
        COMMAND "${CMAKE_COMMAND}"
            "-DSOURCE_DIR=${CMAKE_SOURCE_DIR}/engine/assets"
            "-DDESTINATION_DIR=$<TARGET_FILE_DIR:${target}>/assets"
            "-DEXCLUDED_RELATIVE_DIR=sprites"
            "-DSOURCE_MAP_FILE=$<TARGET_FILE_DIR:${target}>/assets/.asset_sources"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/CopyDirectoryExcept.cmake"
        COMMAND "${CMAKE_COMMAND}"
            "-DSOURCE_DIR=${CMAKE_SOURCE_DIR}/game/assets"
            "-DDESTINATION_DIR=$<TARGET_FILE_DIR:${target}>/assets"
            "-DEXCLUDED_RELATIVE_DIR=sprites"
            "-DSOURCE_MAP_FILE=$<TARGET_FILE_DIR:${target}>/assets/.asset_sources"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/CopyDirectoryExcept.cmake"
        COMMENT "Copying assets to target directory"
        VERBATIM
    )
    add_dependencies(${target} ${target}_CopyAssets)
endfunction()
