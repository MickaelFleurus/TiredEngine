file (GLOB_RECURSE Angine_SRC src/engine/*/*.cpp)
add_library(Angine_LIB ${Angine_SRC})
target_link_libraries(Angine_LIB PUBLIC SDL3::SDL3 imgui glm::glm magic_enum::magic_enum SDL3_ttf::SDL3_ttf nlohmann_json::nlohmann_json)
target_include_directories(Angine_LIB PUBLIC ${CMAKE_SOURCE_DIR}/engine/include)

# Testing
add_subdirectory(tests)