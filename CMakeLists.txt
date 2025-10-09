cmake_minimum_required(VERSION 3.28)
project(GlimmerWorks)

set(CMAKE_CXX_STANDARD 26)
include(FetchContent)
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY "https://github.com/nlohmann/json"
        GIT_TAG "v3.12.0"
        SOURCE_DIR "${PROJECT_SOURCE_DIR}/include/json"
)

FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
        SDL
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
        GIT_TAG "release-3.2.22"
)
FetchContent_MakeAvailable(SDL)

add_executable(GlimmerWorks main.cpp
        core/log/LogCat.cpp
        core/log/LogCat.h
        core/Config.cpp
        core/Config.h)

target_link_libraries(GlimmerWorks PRIVATE SDL3::SDL3)

target_include_directories(GlimmerWorks PRIVATE
        "${PROJECT_SOURCE_DIR}/include/json/include"
)

file(COPY "${CMAKE_SOURCE_DIR}/config.json"
        DESTINATION "${CMAKE_BINARY_DIR}/")

file(WRITE "${CMAKE_BINARY_DIR}/.gitignore" "*")

add_custom_command(TARGET GlimmerWorks POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        "${CMAKE_BINARY_DIR}/libs/libSDL3.so.0"
)