include (GoogleTest)

file(GLOB_RECURSE UI_TESTS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp)

set(UI_TESTS_DEPS_HEADERS
    ${CMAKE_SOURCE_DIR}/include/widgets/uploaded_file_panel.h
)

set(UI_TESTS_DEPS_SOURCES
    ${CMAKE_SOURCE_DIR}/src/widgets/uploaded_file_panel.cpp
)

set(FORMS_DIR ${CMAKE_SOURCE_DIR}/forms)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Add the forms directory to the AUTOUIC search paths
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${FORMS_DIR})

add_executable(ui_tests 
    ${UI_TESTS_DEPS_HEADERS}
    ${UI_TESTS_DEPS_SOURCES}
    ${UI_TESTS_SOURCES}
)

target_link_libraries(ui_tests PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
    GTest::gtest
)

target_include_directories(ui_tests PUBLIC
    ${FORMS_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

set(GTEST_DISCOVER_TESTS_VERBOSE ON)
gtest_discover_tests(ui_tests)