file (GLOB_RECURSE Angine_SRC src/engine/*/*.cpp)
add_library(Angine_LIB STATIC ${Angine_SRC})

target_link_libraries(Angine_LIB PUBLIC 
    SDL3::SDL3 
    imgui 
    glm::glm 
    magic_enum::magic_enum 
    nlohmann_json::nlohmann_json 
    SDL3_image::SDL3_image 
    msdfgen::msdfgen 
    msdf-atlas-gen::msdf-atlas-gen 
    freetype
    Vulkan::Vulkan
    spirv-reflect-static
)

# On macOS, link required frameworks for MoltenVK
if(APPLE)
    find_library(METAL_FRAMEWORK Metal REQUIRED)
    find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
    find_library(QUARTZCORE_FRAMEWORK QuartzCore REQUIRED)
    find_library(IOSURFACE_FRAMEWORK IOSurface REQUIRED)
    
    target_link_libraries(Angine_LIB PUBLIC
        ${METAL_FRAMEWORK}
        ${FOUNDATION_FRAMEWORK}
        ${QUARTZCORE_FRAMEWORK}
        ${IOSURFACE_FRAMEWORK}
    )
endif()

target_include_directories(Angine_LIB PUBLIC ${CMAKE_SOURCE_DIR}/engine/include)

# Testing
add_subdirectory(tests)