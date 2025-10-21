cmake_minimum_required(VERSION 3.26)
project(Game_Angine LANGUAGES CXX)

# Add cmake directory to module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(FetchContent)
include(SDL3)
include(ImGui)
include(glm)
include(MagicEnum)
include(GoogleTest)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

setup_sdl3()
setup_imgui()
setup_magicenum()
setup_glm()
setup_gtest()


# Sources
add_subdirectory(engine)
add_subdirectory(game)

add_executable(BreakoutExclamationMark src/main.cpp)
target_link_libraries(BreakoutExclamationMark PRIVATE Angine_LIB Breakout_LIB)

if(TARGET CompileShaders)
    add_dependencies(BreakoutExclamationMark CompileShaders)
endif()

if(WIN32)
    add_custom_command(TARGET BreakoutExclamationMark POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/_deps/sdl3-build/Debug/SDL3.dll,${CMAKE_BINARY_DIR}/_deps/sdl3-build/Release/SDL3.dll>"
        "$<TARGET_FILE_DIR:BreakoutExclamationMark>"
        COMMENT "Copying SDL3.dll to output directory"
    )
endif()

# Copy compiled shaders to the executable directory after build
add_custom_command(TARGET BreakoutExclamationMark POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_BINARY_DIR}/shaders"
    "$<TARGET_FILE_DIR:BreakoutExclamationMark>/shaders"
    COMMENT "Copying compiled shaders to output directory"
)