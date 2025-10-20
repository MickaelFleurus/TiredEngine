
file(GLOB_RECURSE TEST_SOURCES *.cpp)

enable_testing()
add_executable(angine_tests ${TEST_SOURCES})

target_link_libraries(angine_tests PRIVATE SDL3::SDL3 imgui GTest::gtest_main Angine_LIB)
target_include_directories(angine_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Use add_test instead of gtest_discover_tests if the module isn't available
add_test(NAME angine_tests COMMAND angine_tests)