cmake_minimum_required(VERSION 3.26)
project(Game_Angine LANGUAGES CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Vulkan
find_package(Vulkan REQUIRED) 

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
set(IMGUI_SOURCES
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${CMAKE_SOURCE_DIR}/external/imgui)

# GoogleTest
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
    SOURCE_DIR external/googletest
)
FetchContent_MakeAvailable(googletest)

# Shaders
add_subdirectory(shaders)

# Testing
add_subdirectory(tests)

# Sources
file (GLOB_RECURSE Angine_SRC src/*.cpp)

add_executable(Game_Angine ${Angine_SRC})

include_directories(${SDL3_INCLUDE_DIRS} include)
target_link_libraries(Game_Angine PRIVATE Vulkan::Vulkan SDL3::SDL3 imgui)

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