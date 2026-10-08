include(ExternalProject)

set(TIREDPACKER_SOURCE_DIR
    "${CMAKE_SOURCE_DIR}/tools/TiredPacker"
)

set(TIREDPACKER_BINARY_DIR
    "${CMAKE_BINARY_DIR}/TiredPacker-build"
)

set(TIREDPACKER_EXECUTABLE
    "${TIREDPACKER_BINARY_DIR}/TiredPacker${CMAKE_EXECUTABLE_SUFFIX}"
)

set(GAME_SPRITES_ASSETS_DIR
    "${CMAKE_BINARY_DIR}/assets/sprites/"
)

set(TIREDPACKER_ATLAS_OUTPUT_DIR
    "${CMAKE_BINARY_DIR}/assets/sprites/"
)

ExternalProject_Add(TiredPackerExternal
    SOURCE_DIR "${TIREDPACKER_SOURCE_DIR}"
    BINARY_DIR "${TIREDPACKER_BINARY_DIR}"

    BUILD_BYPRODUCTS "${TIREDPACKER_EXECUTABLE}"

    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=Release

    BUILD_COMMAND
        "${CMAKE_COMMAND}" --build
        <BINARY_DIR>
        --config Release

    INSTALL_COMMAND ""
)

add_custom_target(GenerateAtlases
    COMMAND "${TIREDPACKER_EXECUTABLE}"
        "${CMAKE_SOURCE_DIR}/game/assets/sprites"
        -o "${TIREDPACKER_ATLAS_OUTPUT_DIR}" 
        
    COMMAND "${TIREDPACKER_EXECUTABLE}"
        "${CMAKE_SOURCE_DIR}/engine/assets/sprites"
        -o "${TIREDPACKER_ATLAS_OUTPUT_DIR}"
    DEPENDS TiredPackerExternal
    COMMENT "Generating texture atlases"
    VERBATIM
)


