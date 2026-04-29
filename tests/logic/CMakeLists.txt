include(GoogleTest)

file(GLOB_RECURSE LOGIC_TESTS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp)

add_executable(logic_tests ${LOGIC_TESTS_SOURCES})

target_link_libraries(logic_tests PRIVATE
    core
    GTest::gtest
    GTest::gtest_main
)

# Copy test fixtures to build/tests/tmp directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tmp)

add_custom_command(
    TARGET logic_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/tests/fixtures/test.jpg
        ${CMAKE_CURRENT_BINARY_DIR}/tmp/test.jpg
    COMMENT "Copying test fixtures"
)

set(GTEST_DISCOVER_TESTS_VERBOSE ON)
gtest_discover_tests(logic_tests)
