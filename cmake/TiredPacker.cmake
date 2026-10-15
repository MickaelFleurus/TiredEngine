set(TIREDPACKER_SOURCE_DIR
    "${CMAKE_SOURCE_DIR}/tools/TiredPacker"
)

set(TIREDPACKER_BINARY_DIR
    "${CMAKE_BINARY_DIR}/TiredPacker-build"
)

set(GAME_SPRITES_ASSETS_DIR
    "${CMAKE_BINARY_DIR}/assets/sprites/"
)

set(TIREDPACKER_ATLAS_OUTPUT_DIR
    "${CMAKE_BINARY_DIR}/assets/sprites/"
)

add_subdirectory("${TIREDPACKER_SOURCE_DIR}" "${TIREDPACKER_BINARY_DIR}")

add_custom_target(GenerateAtlases
    COMMAND $<TARGET_FILE:TiredPacker>
        "${CMAKE_SOURCE_DIR}/game/assets/sprites"
        -o "${TIREDPACKER_ATLAS_OUTPUT_DIR}" 
        
    COMMAND $<TARGET_FILE:TiredPacker>
        "${CMAKE_SOURCE_DIR}/engine/assets/sprites"
        -o "${TIREDPACKER_ATLAS_OUTPUT_DIR}"
    DEPENDS TiredPacker
    COMMENT "Generating texture atlases"
    VERBATIM
)


