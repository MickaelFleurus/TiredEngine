# Minimal CMakeLists.txt for tests
file(GLOB_RECURSE TEST_SOURCES *.cpp)

enable_testing()
add_executable(angine_tests ${TEST_SOURCES})

target_link_libraries(angine_tests PRIVATE Vulkan::Vulkan SDL3::SDL3 imgui GTest::gtest_main Angine_LIB)
target_include_directories(angine_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

include(GoogleTest)
gtest_discover_tests(angine_tests)