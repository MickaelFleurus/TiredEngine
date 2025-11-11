file (GLOB_RECURSE Breakout_SRC src/*/*.cpp)

add_library(Breakout_LIB STATIC ${Breakout_SRC})
target_link_libraries(Breakout_LIB PUBLIC Angine_LIB)
target_include_directories(Breakout_LIB PUBLIC ${CMAKE_SOURCE_DIR}/game/include)
