file (GLOB_RECURSE Breakout_SRC src/*/*.cpp)
add_library(Breakout_LIB ${Breakout_SRC})
target_link_libraries(Breakout_LIB PUBLIC SDL3::SDL3 imgui glm::glm magic_enum::magic_enum Angine_LIB)
target_include_directories(Breakout_LIB PRIVATE ${CMAKE_SOURCE_DIR}/game/include)

# Shaders
add_subdirectory(assets/shaders)