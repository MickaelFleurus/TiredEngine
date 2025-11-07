cmake_minimum_required(VERSION 3.26)
project(Game_Angine LANGUAGES CXX)

# Add cmake directory to module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(FetchContent)
include(Brotli)
include(PNGLib)
include(SDL3)
include(SDL_ttf)
include(SDL3_image)
include(ImGui)
include(glm)
include(MagicEnum)
include(GoogleTest)
include(nlohmann)
include(freetype)
include(msdfgen)
include(msdf-atlas-gen)
include(CompileShaders)
include(AssetsCopy)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

setup_pnglib()
setup_brotli()
setup_sdl3()
setup_sdl3_image()
setup_imgui()
setup_magicenum()
setup_glm()
setup_gtest()
setup_nlohmann_json()
setup_freetype()
setup_msdfgen()
setup_msdfatlasgen()

add_subdirectory(engine)
add_subdirectory(game)


# Compile all shaders from both engine and game
compile_shaders("${CMAKE_BINARY_DIR}/assets/shaders" CompileShaders)

add_executable(BreakoutExclamationMark src/main.cpp)
target_link_libraries(BreakoutExclamationMark PRIVATE Angine_LIB Breakout_LIB)

add_dependencies(BreakoutExclamationMark CopyAssets)
add_dependencies(BreakoutExclamationMark CompileShaders)

if(WIN32)
    add_custom_command(TARGET BreakoutExclamationMark POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/_deps/sdl3-build/Debug/SDL3.dll,${CMAKE_BINARY_DIR}/_deps/sdl3-build/Release/SDL3.dll>"
        "$<TARGET_FILE_DIR:BreakoutExclamationMark>"
        COMMENT "Copying SDL3.dll to output directory"
    )
endif()