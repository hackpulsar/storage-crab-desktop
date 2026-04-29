cmake_minimum_required(VERSION 3.29)
project(storage-crab-desktop)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

file(GLOB_RECURSE SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp
)
file(GLOB_RECURSE HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*/*.hpp
)
file(GLOB_RECURSE FORMS
        ${CMAKE_CURRENT_SOURCE_DIR}/forms/*.ui
)

# libcurl (cURLpp dependency)
find_package(CURL REQUIRED)

# cURLpp itself
FetchContent_Declare(
        curlpp
        GIT_REPOSITORY https://github.com/jpbarrette/curlpp
        GIT_SHALLOW ON
)
FetchContent_MakeAvailable(curlpp)

# nlohmann JSON
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_SHALLOW ON
)
FetchContent_MakeAvailable(nlohmann_json)

# Google Test
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(googletest)

# Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Concurrent Test)

# Setup sources directories
set(FORMS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/forms)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Add the forms directory to the AUTOUIC search paths
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${FORMS_DIR})

# OpenSSL
find_package(OpenSSL REQUIRED)

# dotenv
add_subdirectory(deps)

# Add the executable based on platform
if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} ${FORMS})
elseif(UNIX)
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${FORMS})
endif()

target_link_libraries(${PROJECT_NAME}
        PRIVATE
            Qt6::Widgets
            Qt6::Concurrent
            ${CURL_LIBRARIES}
            curlpp
            nlohmann_json::nlohmann_json
            ${OPENSSL_LIBRARIES}
            dotenv
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
            ${FORMS_DIR}
            ${SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${OPENSSL_INCLUDE_DIR}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    ASSETS_PATH="${CMAKE_SOURCE_DIR}/assets"
)

add_subdirectory(tests)