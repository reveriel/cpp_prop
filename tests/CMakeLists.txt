# Add the test executable
add_executable(logic_tests test_logic.cpp)

# Link the test executable against Google Test
target_link_libraries(logic_tests GTest::gtest_main)

# Add the test to CTest
include(GoogleTest)
gtest_discover_tests(logic_tests)
