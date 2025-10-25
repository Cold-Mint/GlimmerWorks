cmake_minimum_required(VERSION 3.28)
project(GlimmerWorks)

set(CMAKE_CXX_STANDARD 26)
set(GAME_VERSION_MAJOR 0)
set(GAME_VERSION_MINOR 1)
set(GAME_VERSION_PATCH 0)
set(GAME_VERSION_STRING "${GAME_VERSION_MAJOR}.${GAME_VERSION_MINOR}.${GAME_VERSION_PATCH}")
set(GAME_VERSION_NUMBER 1)
set(IMGUI_VERSION "1.92.3")
set(TWEENY_VERSION "3.2.1")
set(FETCHCONTENT_QUIET OFF CACHE BOOL "Show real-time git clone progress (ON=quiet, OFF=visible)" FORCE)
set(CMAKE_MESSAGE_LOG_LEVEL STATUS CACHE STRING
        "CMake message verbosity level")
set(CONFIG_FILE_NAME "config.json")
set(CMAKE_BUILD_RPATH "$ORIGIN/libs")
set(CMAKE_INSTALL_RPATH "$ORIGIN/libs")
include(FetchContent)
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY "https://github.com/nlohmann/json"
        GIT_TAG "v3.12.0"
        GIT_PROGRESS TRUE
        SOURCE_DIR "${PROJECT_SOURCE_DIR}/include/json"
)

FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
        SDL
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
        GIT_TAG "release-3.2.22"
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL)

FetchContent_Declare(
        SDL_image
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL_image.git"
        GIT_TAG "release-3.2.4"
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL_image)


FetchContent_Declare(
        SDL_ttf
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL_ttf.git"
        GIT_TAG "release-3.2.2"
)
FetchContent_MakeAvailable(SDL_ttf)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
        GIT_PROGRESS TRUE
        GIT_TAG "v${IMGUI_VERSION}"
        SOURCE_DIR "${PROJECT_SOURCE_DIR}/include/imgui-${IMGUI_VERSION}"
)

FetchContent_MakeAvailable(imgui)
FetchContent_Declare(
        tweeny
        GIT_REPOSITORY "https://github.com/mobius3/tweeny.git"
        GIT_PROGRESS TRUE
        GIT_TAG "v${TWEENY_VERSION}"
        SOURCE_DIR "${PROJECT_SOURCE_DIR}/include/tweeny-${TWEENY_VERSION}"
)

FetchContent_MakeAvailable(tweeny)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
        GIT_PROGRESS TRUE
        GIT_TAG "12.0.0"
)
FetchContent_MakeAvailable(fmt)

add_executable(GlimmerWorks "${PROJECT_SOURCE_DIR}/include/imgui-${IMGUI_VERSION}/imgui.cpp"
        "${PROJECT_SOURCE_DIR}/include/imgui-${IMGUI_VERSION}/imgui_draw.cpp"
        "${PROJECT_SOURCE_DIR}/include/imgui-${IMGUI_VERSION}/imgui_widgets.cpp"
        "${PROJECT_SOURCE_DIR}/include/imgui-${IMGUI_VERSION}/imgui_tables.cpp"
        "${PROJECT_SOURCE_DIR}/include/imgui-${IMGUI_VERSION}/imgui_demo.cpp"
        # SDL3 backend
        # SDL3 后端
        "${PROJECT_SOURCE_DIR}/include/imgui-${IMGUI_VERSION}/backends/imgui_impl_sdl3.cpp"
        "${PROJECT_SOURCE_DIR}/include/imgui-${IMGUI_VERSION}/backends/imgui_impl_sdlrenderer3.cpp"
        "${PROJECT_SOURCE_DIR}/include/tweeny-${TWEENY_VERSION}/include/tween.h"

        main.cpp
        core/log/LogCat.cpp
        core/log/LogCat.h
        core/Config.cpp
        core/Config.h
        core/App.cpp
        core/App.h
        core/mod/dataPack/StringManager.cpp
        core/mod/dataPack/StringManager.h
        core/mod/Resource.cpp
        core/mod/Resource.h
        core/mod/dataPack/DataPackManager.cpp
        core/mod/dataPack/DataPackManager.h
        core/mod/dataPack/DataPack.cpp
        core/mod/dataPack/DataPack.h
        core/mod/PackManifest.cpp
        core/mod/PackManifest.h
        core/Constants.h
        core/utils/JsonUtils.cpp
        core/utils/JsonUtils.h
        core/utils/LanguageUtils.cpp
        core/utils/LanguageUtils.h
        core/mod/ResourceRef.cpp
        core/mod/ResourceRef.h
        core/scene/Scene.cpp
        core/scene/Scene.h
        core/scene/AppContext.cpp
        core/scene/AppContext.h
        core/scene/SplashScene.cpp
        core/scene/SplashScene.h
        core/scene/ConsoleScene.cpp
        core/scene/ConsoleScene.h
        core/console/Command.cpp
        core/console/Command.h
        core/console/CommandExecutor.cpp
        core/console/CommandExecutor.h
        core/console/CommandManager.cpp
        core/console/CommandManager.h
        core/console/CommandArgs.cpp
        core/console/CommandArgs.h
        core/console/CommandResult.cpp
        core/console/CommandResult.h
        core/mod/resourcePack/ResourcePack.cpp
        core/mod/resourcePack/ResourcePack.h
        core/mod/resourcePack/ResourcePackManager.cpp
        core/mod/resourcePack/ResourcePackManager.h
        core/console/command/HelpCommand.cpp
        core/console/command/HelpCommand.h
        core/scene/SceneManager.cpp
        core/scene/SceneManager.h
        core/scene/HomeScene.cpp
        core/scene/HomeScene.h
        core/Langs.cpp
        core/Langs.h
        core/scene/SavedGamesScene.cpp
        core/scene/SavedGamesScene.h
        core/saves/Saves.cpp
        core/saves/Saves.h
        core/saves/MapManifest.cpp
        core/saves/MapManifest.h
        core/scene/CreateWorldScene.cpp
        core/scene/CreateWorldScene.h
        core/world/WorldGenerator.cpp
        core/world/WorldGenerator.h
        core/world/WorldGeneratorTask.cpp
        core/world/WorldGeneratorTask.h
        core/world/Chunk.cpp
        core/world/Chunk.h
        core/world/WorldContext.cpp
        core/world/WorldContext.h
        core/math/Vector2D.cpp
        core/math/Vector2D.h
        core/math/FastNoiseLite.h
        core/world/GenerateChunkTask.cpp
        core/world/GenerateChunkTask.h
        core/scene/WorldScene.cpp
        core/scene/WorldScene.h
)

#Define version information
#定义版本信息
target_compile_definitions(GlimmerWorks PRIVATE
        GAME_VERSION_MAJOR=${GAME_VERSION_MAJOR}
        GAME_VERSION_MINOR=${GAME_VERSION_MINOR}
        GAME_VERSION_PATCH=${GAME_VERSION_PATCH}
        GAME_VERSION_STRING="${GAME_VERSION_STRING}"
        GAME_VERSION_NUMBER=${GAME_VERSION_NUMBER}
        CONFIG_FILE_NAME="${CONFIG_FILE_NAME}"
)

target_link_libraries(GlimmerWorks PRIVATE SDL3::SDL3 SDL3_image::SDL3_image fmt::fmt SDL3_ttf::SDL3_ttf)

target_include_directories(GlimmerWorks PRIVATE
        "${PROJECT_SOURCE_DIR}/include/json/include"
)

target_include_directories(GlimmerWorks PRIVATE
        "${PROJECT_SOURCE_DIR}/include/imgui-${IMGUI_VERSION}"
)

target_include_directories(GlimmerWorks PRIVATE
        "${PROJECT_SOURCE_DIR}/include/tweeny-${TWEENY_VERSION}"
)

file(COPY "${CMAKE_SOURCE_DIR}/${CONFIG_FILE_NAME}"
        DESTINATION "${CMAKE_BINARY_DIR}/")

file(COPY "${CMAKE_SOURCE_DIR}/core_langs/"
        DESTINATION "${CMAKE_BINARY_DIR}/langs/")

file(WRITE "${CMAKE_BINARY_DIR}/.gitignore" "*")

# Generate build.sh in the build directory
# 在构建目录生成 build.sh
set(BUILD_SCRIPT "${CMAKE_BINARY_DIR}/build.sh")
file(WRITE "${BUILD_SCRIPT}" "#!/bin/bash\n")
file(APPEND "${BUILD_SCRIPT}" "set -e\n")
file(APPEND "${BUILD_SCRIPT}" "GAME_VERSION=\"${GAME_VERSION_STRING}\"\n")
file(APPEND "${BUILD_SCRIPT}" "OUTPUT_FILE=\"GlimmerWorks_\${GAME_VERSION}_linux.zip\"\n")
file(APPEND "${BUILD_SCRIPT}" "echo \"Packing files into \${OUTPUT_FILE}...\"\n")
# Exclude all hidden files and folders
# 排除所有隐藏文件和文件夹
file(APPEND "${BUILD_SCRIPT}" "zip -r \${OUTPUT_FILE} config.json GlimmerWorks langs libs mods -x '*/.*'\n")
file(APPEND "${BUILD_SCRIPT}" "echo \"Done!\"\n")


add_custom_command(TARGET GlimmerWorks POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        "${CMAKE_BINARY_DIR}/libs/libSDL3.so.0"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3_image::SDL3_image>
        "${CMAKE_BINARY_DIR}/libs/libSDL3_image.so.0"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
        "${CMAKE_BINARY_DIR}/libs/libSDL3_ttf.so.0"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:fmt::fmt>
        "${CMAKE_BINARY_DIR}/libs/libfmtd.so.12"
)

#Clone data packets and resource packets.
#克隆数据包和资源包
FetchContent_Declare(
        VanillaDataPack
        GIT_REPOSITORY "https://github.com/Cold-Mint/glimmerworks-data.git"
        GIT_PROGRESS TRUE
        SOURCE_DIR "${CMAKE_BINARY_DIR}/mods/data_packs/vanilla"
)

FetchContent_GetProperties(VanillaDataPack)
if (NOT VanillaDataPack_POPULATED)
    FetchContent_MakeAvailable(VanillaDataPack)
endif ()

FetchContent_Declare(
        VanillaResourcePack
        GIT_REPOSITORY "https://github.com/Cold-Mint/glimmerworks-assets.git"
        GIT_PROGRESS TRUE
        SOURCE_DIR "${CMAKE_BINARY_DIR}/mods/resource_packs/vanilla"
)

FetchContent_GetProperties(VanillaResourcePack)
if (NOT VanillaDataPack_POPULATED)
    FetchContent_Populate(VanillaResourcePack)
endif ()
