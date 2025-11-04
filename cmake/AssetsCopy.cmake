add_custom_target(CopyAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/engine/assets"
        "${CMAKE_BINARY_DIR}/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/game/assets"
        "${CMAKE_BINARY_DIR}/assets"
    COMMENT "Copying all assets to output directory before build"
    VERBATIM
)