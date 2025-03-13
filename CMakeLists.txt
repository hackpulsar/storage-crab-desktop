cmake_minimum_required(VERSION 3.29)
project(storage-crab-desktop)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
file(GLOB_RECURSE FORMS ${CMAKE_CURRENT_SOURCE_DIR}/forms/*.ui)

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

# Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Concurrent)

# Setup sources directories
set(FORMS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/forms)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Add the forms directory to the AUTOUIC search paths
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${FORMS_DIR})

# Add the executable based on platform
if (WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})
elseif(UNIX)
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
endif()

target_link_libraries(${PROJECT_NAME}
        PRIVATE
            Qt6::Widgets
            Qt6::Concurrent
            ${CURL_LIBRARIES}
            curlpp
            nlohmann_json::nlohmann_json
)

target_include_directories( ${PROJECT_NAME}
        PUBLIC
            ${FORMS_DIR}
            ${SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/include
)
