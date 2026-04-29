include (GoogleTest)

file(GLOB_RECURSE UI_TESTS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp)

file(GLOB_RECURSE UI_TESTS_DEPS_HEADERS
    ${CMAKE_SOURCE_DIR}/include/*/*.h
    ${CMAKE_SOURCE_DIR}/include/*/*.hpp
)

file(GLOB_RECURSE UI_TESTS_DEPS_SOURCES
    ${CMAKE_SOURCE_DIR}/src/*/*.cpp
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
    Qt6::Concurrent
    Qt6::Test
    GTest::gtest
    curlpp
    dotenv
    ${CURL_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    nlohmann_json::nlohmann_json
)

target_include_directories(ui_tests PUBLIC
    ${FORMS_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_definitions(ui_tests PRIVATE
    ASSETS_PATH="${CMAKE_SOURCE_DIR}/assets"
)

set(GTEST_DISCOVER_TESTS_VERBOSE ON)
gtest_discover_tests(ui_tests)