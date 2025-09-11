# Minimal CMakeLists.txt for tests
file(GLOB TEST_SOURCES *.cpp)

enable_testing()
add_executable(angine_tests ${TEST_SOURCES})

target_link_libraries(angine_tests PRIVATE Vulkan::Vulkan SDL3::SDL3 imgui gtest gtest_main)
target_include_directories(angine_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_test(NAME AngineTests COMMAND angine_tests)
