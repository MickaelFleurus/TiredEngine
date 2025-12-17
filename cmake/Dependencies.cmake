include(FetchContent)

include(Brotli)
include(zlib)
include(PNGLib)
include(SDL3)
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
include(Vulkan)
include(SPIRV-Reflect)
include(yaml-cpp)

function(setup_dependencies)
    # Base dependencies first (no external deps)
    setup_vulkan()
    setup_spirv_reflect()
    setup_zlib()
    setup_pnglib() 
    setup_brotli()
    setup_glm()
    setup_magicenum()
    setup_nlohmann_json()
    setup_gtest()
    setup_yaml_cpp()
    
    # Libraries that depend on base libraries
    setup_freetype()    # depends on zlib, brotli
    
    # SDL libraries
    setup_sdl3()
    setup_sdl3_image() # depends on SDL3
    
    # Libraries that depend on SDL/graphics
    setup_imgui()       # depends on SDL3
    
    # Font/text libraries that depend on FreeType
    setup_msdfgen()     # depends on FreeType
    setup_msdfatlasgen() # depends on msdfgen, FreeType
endfunction()
