include (GoogleTest)

file(GLOB_RECURSE UI_TESTS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp)

add_executable(ui_tests ${UI_TESTS_SOURCES})

target_link_libraries(ui_tests PRIVATE
    ui_elements
    Qt6::Test
    GTest::gtest
)

target_compile_definitions(ui_tests PRIVATE
    ASSETS_PATH="${CMAKE_SOURCE_DIR}/assets"
)

set(GTEST_DISCOVER_TESTS_VERBOSE ON)
gtest_discover_tests(ui_tests)