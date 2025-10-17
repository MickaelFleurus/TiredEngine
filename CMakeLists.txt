cmake_minimum_required(VERSION 3.26)
project(Game_Angine LANGUAGES CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find or download SDL3
set(SDL3_SHARED ON CACHE BOOL "" FORCE)
set(SDL3_STATIC OFF CACHE BOOL "" FORCE)
find_package(SDL3 QUIET)
if(NOT SDL3_FOUND)
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.22
        SOURCE_DIR external/SDL3
    )
    FetchContent_MakeAvailable(SDL3)
endif()

# imgui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.3
    SOURCE_DIR external/imgui
)
FetchContent_MakeAvailable(imgui)
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_link_libraries(imgui PRIVATE SDL3::SDL3)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)

# GoogleTest
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    SOURCE_DIR external/googletest
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Shaders
add_subdirectory(shaders)

# Testing
add_subdirectory(tests)

# Sources
file (GLOB_RECURSE Angine_SRC src/*/*.cpp)
add_library(Angine_LIB ${Angine_SRC})
target_link_libraries(Angine_LIB PRIVATE SDL3::SDL3 imgui)
target_include_directories(Angine_LIB PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(Game_Angine src/main.cpp)
target_link_libraries(Game_Angine PRIVATE Angine_LIB)
target_include_directories(Game_Angine PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Post-build commands

if(WIN32)
    add_custom_command(TARGET Game_Angine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/_deps/sdl3-build/Debug/SDL3.dll,${CMAKE_BINARY_DIR}/_deps/sdl3-build/Release/SDL3.dll>"
        "$<TARGET_FILE_DIR:Game_Angine>"
        COMMENT "Copying SDL3.dll to output directory"
    )
endif()

# Copy compiled shaders to the executable directory after build
add_custom_command(TARGET Game_Angine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_BINARY_DIR}/shaders"
    "$<TARGET_FILE_DIR:Game_Angine>/shaders"
    COMMENT "Copying compiled shaders to output directory"
)