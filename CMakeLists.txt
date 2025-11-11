cmake_minimum_required(VERSION 3.26)
project(Game_Angine LANGUAGES CXX)

# Add cmake directory to module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
if(WIN32 AND MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include (Dependencies)
setup_dependencies()

include (CopyDLLs)

add_subdirectory(engine)
add_subdirectory(game)

add_executable(BreakoutExclamationMark src/main.cpp)
target_link_libraries(BreakoutExclamationMark PRIVATE Angine_LIB Breakout_LIB)


copy_dll_dependencies(BreakoutExclamationMark)
compile_shaders_to_target(BreakoutExclamationMark)
copy_assets_to_target(BreakoutExclamationMark)
