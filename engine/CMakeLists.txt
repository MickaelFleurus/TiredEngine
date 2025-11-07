file (GLOB_RECURSE Angine_SRC src/engine/*/*.cpp)
add_library(Angine_LIB ${Angine_SRC})
target_link_libraries(Angine_LIB PUBLIC SDL3::SDL3 imgui glm::glm magic_enum::magic_enum nlohmann_json::nlohmann_json SDL3_image::SDL3_image msdfgen::msdfgen msdf-atlas-gen::msdf-atlas-gen freetype)
target_include_directories(Angine_LIB PUBLIC ${CMAKE_SOURCE_DIR}/engine/include)

# Testing
add_subdirectory(tests)