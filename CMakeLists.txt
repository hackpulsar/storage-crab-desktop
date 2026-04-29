cmake_minimum_required(VERSION 3.29)
project(storage-crab-desktop)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# libcurl (cURLpp dependency)
find_package(CURL REQUIRED)

# cURLpp itself
FetchContent_Declare(
    curlpp
    GIT_REPOSITORY https://github.com/jpbarrette/curlpp
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(curlpp)
# suppress warnings
target_compile_options(curlpp PRIVATE -Wno-deprecated-declarations)
set_target_properties(curlpp PROPERTIES
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${curlpp_SOURCE_DIR}/include"
)

# nlohmann_json
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

# OpenSSL
find_package(OpenSSL REQUIRED)

# dotenv
add_subdirectory(deps)

# Core library

file(GLOB_RECURSE CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/api/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cryptography/*.cpp
)

file(GLOB_RECURSE CORE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/api/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/api/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cryptography/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cryptography/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils/*.hpp
)

add_library(core STATIC
    ${CORE_SOURCES}
    ${CORE_HEADERS}
)

target_link_libraries(core
    PUBLIC
        Qt6::Core
        Qt6::Concurrent
        dotenv
        curlpp
        nlohmann_json::nlohmann_json
        ${CURL_LIBRARIES}
        ${OPENSSL_LIBRARIES}
)

set_target_properties(core PROPERTIES
    AUTOMOC ON
)

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# UI

file(GLOB_RECURSE UI_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/windows/*.cpp
)

file(GLOB_RECURSE UI_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/windows/*.h
)

file(GLOB_RECURSE FORMS ${CMAKE_CURRENT_SOURCE_DIR}/forms/*.ui)
set(FORMS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/forms)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${FORMS_DIR})

add_library(ui_elements STATIC
    ${UI_SOURCES}
    ${UI_HEADERS}
)

set_target_properties(ui_elements PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

target_link_libraries(ui_elements PUBLIC
    core
    Qt6::Widgets
)

target_include_directories(ui_elements PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${FORMS_DIR}
)

target_compile_definitions(ui_elements PRIVATE
    ASSETS_PATH="${CMAKE_SOURCE_DIR}/assets"
)

# App
file(GLOB APP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${APP_SOURCES} ${APP_HEADERS} ${FORMS})
elseif(UNIX)
    add_executable(${PROJECT_NAME} ${APP_SOURCES} ${FORMS})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    core
    ui_elements
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests)