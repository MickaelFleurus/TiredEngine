file (GLOB_RECURSE Breakout_SRC src/*/*.cpp)
message(STATUS "Breakout source files: ${Breakout_SRC}")

add_library(Breakout_LIB ${Breakout_SRC})
target_link_libraries(Breakout_LIB PUBLIC Angine_LIB)
target_include_directories(Breakout_LIB PUBLIC ${CMAKE_SOURCE_DIR}/game/include)

# Shaders
add_subdirectory(assets/shaders)
