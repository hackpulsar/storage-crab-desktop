include(GoogleTest)

file(GLOB_RECURSE TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/logic/*_test.cpp)

set(TEST_DEPS_SOURCES
    ${CMAKE_SOURCE_DIR}/src/api/token_pair.cpp
    ${CMAKE_SOURCE_DIR}/src/cryptography/aes.cpp
    ${CMAKE_SOURCE_DIR}/src/cryptography/rsa.cpp
    ${CMAKE_SOURCE_DIR}/src/cryptography/crypto_service.cpp
    ${CMAKE_SOURCE_DIR}/src/cryptography/file_crypto.cpp
)

add_executable(tests ${TEST_DEPS_SOURCES} ${TEST_SOURCES})

target_link_libraries(tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    nlohmann_json::nlohmann_json
    curlpp
    ${CURL_LIBRARIES}
    dotenv
    Qt6::Core
    ${OPENSSL_LIBRARIES}
)

target_include_directories(tests PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/deps/dotenv-cpp/include/laserpants
    ${CURL_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
)

# Copy test fixtures to build/tests/tmp directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tmp)

add_custom_target(test_fixtures ALL)

add_custom_command(
    TARGET test_fixtures
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/tests/fixtures/test.jpg
        ${CMAKE_CURRENT_BINARY_DIR}/tmp/test.jpg
    COMMENT "Copying test fixtures"
)

set(GTEST_DISCOVER_TESTS_VERBOSE ON)
gtest_discover_tests(tests)
