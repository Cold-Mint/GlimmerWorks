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
if (WIN32)
    set(PLATFORM_NAME "windows")
elseif (APPLE)
    set(PLATFORM_NAME "macos")
elseif (ANDROID)
    set(PLATFORM_NAME "android")
elseif (UNIX)
    set(PLATFORM_NAME "linux")
else ()
    set(PLATFORM_NAME "unknown")
endif ()
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
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
        GIT_TAG "release-3.2.26"
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL)

FetchContent_Declare(
        SDL_image
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL_image"
        GIT_TAG "release-3.2.4"
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL_image)


FetchContent_Declare(
        SDL_ttf
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL_ttf"
        GIT_TAG "release-3.2.2"
        GIT_PROGRESS TRUE
)
set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL_ttf)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY "https://github.com/ocornut/imgui"
        GIT_PROGRESS TRUE
        GIT_TAG "v${IMGUI_VERSION}"
        SOURCE_DIR "${PROJECT_SOURCE_DIR}/include/imgui-${IMGUI_VERSION}"
)

FetchContent_MakeAvailable(imgui)
FetchContent_Declare(
        tweeny
        GIT_REPOSITORY "https://github.com/mobius3/tweeny"
        GIT_PROGRESS TRUE
        GIT_TAG "v${TWEENY_VERSION}"
        SOURCE_DIR "${PROJECT_SOURCE_DIR}/include/tweeny-${TWEENY_VERSION}"
)

FetchContent_MakeAvailable(tweeny)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY "https://github.com/fmtlib/fmt"
        GIT_PROGRESS TRUE
        GIT_TAG "12.0.0"
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
        fastNoiseLite
        GIT_REPOSITORY "https://github.com/Auburn/FastNoiseLite"
        GIT_PROGRESS TRUE
        GIT_TAG "v1.1.1"
        SOURCE_DIR "${PROJECT_SOURCE_DIR}/include/fastNoiseLite"
)
FetchContent_MakeAvailable(fastNoiseLite)

FetchContent_Declare(
        box2d
        GIT_REPOSITORY "https://github.com/erincatto/box2d"
        GIT_TAG "v3.1.1"
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(box2d)

set(GLIMMERWORKS_SOURCES "${PROJECT_SOURCE_DIR}/include/imgui-${IMGUI_VERSION}/imgui.cpp"
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
        core/scene/ConsoleOverlay.cpp
        core/scene/ConsoleOverlay.h
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
        core/world/WorldContext.cpp
        core/world/WorldContext.h
        core/math/Vector2D.cpp
        core/math/Vector2D.h
        core/scene/WorldScene.cpp
        core/scene/WorldScene.h
        core/scene/DebugOverlay.cpp
        core/scene/DebugOverlay.h
        core/ecs/GameEntity.cpp
        core/ecs/GameEntity.h
        core/ecs/GameComponent.cpp
        core/ecs/GameComponent.h
        core/ecs/GameSystem.cpp
        core/ecs/GameSystem.h
        core/ecs/system/Transform2DSystem.cpp
        core/ecs/system/Transform2DSystem.h
        core/ecs/component/Transform2DComponent.cpp
        core/ecs/component/Transform2DComponent.h
        core/ecs/system/GameStartSystem.cpp
        core/ecs/system/GameStartSystem.h
        core/ecs/system/CameraSystem.cpp
        core/ecs/system/PlayerControlSystem.cpp
        core/ecs/component/CameraComponent.cpp
        core/ecs/component/CameraComponent.h
        core/ecs/component/DebugDrawComponent.cpp
        core/ecs/component/DebugDrawComponent.h
        core/ecs/system/DebugDrawSystem.cpp
        core/ecs/system/DebugDrawSystem.h
        core/ecs/component/TileLayerComponent.cpp
        core/ecs/component/TileLayerComponent.h
        core/ecs/system/TileLayerSystem.cpp
        core/ecs/system/TileLayerSystem.h
        core/world/Tile.cpp
        core/world/Tile.h
        core/math/Vector2DI.cpp
        core/math/Vector2DI.h
        core/ecs/system/DebugPanelSystem.cpp
        core/ecs/system/DebugPanelSystem.h
        core/ecs/system/ChunkSystem.cpp
        core/ecs/system/ChunkSystem.h
        core/world/Chunk.cpp
        core/world/Chunk.h
        core/ecs/component/RigidBody2DComponent.cpp
        core/ecs/component/RigidBody2DComponent.h
        core/ecs/system/PhysicsSystem.cpp
        core/ecs/system/PhysicsSystem.h
        core/ecs/system/DebugDrawBox2dSystem.cpp
        core/ecs/system/DebugDrawBox2dSystem.h
        core/utils/Box2DUtils.cpp
        core/utils/Box2DUtils.h
        core/world/ChunkPhysicsHelper.cpp
        core/world/ChunkPhysicsHelper.h
        core/console/command/TpCommand.cpp
        core/console/command/TpCommand.h
        core/console/command/Box2DCommand.cpp
        core/console/command/Box2DCommand.h
        core/utils/NodeTree.cpp
        core/utils/NodeTree.h
        core/console/command/AssetViewerCommand.cpp
        core/console/command/AssetViewerCommand.h
        core/console/suggestion/DynamicSuggestionsManager.cpp
        core/console/suggestion/DynamicSuggestionsManager.h
        core/console/suggestion/DynamicSuggestions.cpp
        core/console/suggestion/DynamicSuggestions.h
        core/console/suggestion/BoolDynamicSuggestions.cpp
        core/console/suggestion/BoolDynamicSuggestions.h
        core/vfs/VirtualFileSystem.cpp
        core/vfs/VirtualFileSystem.h
        core/vfs/FileProvider.cpp
        core/vfs/FileProvider.h
        core/vfs/StdFileProvider.cpp
        core/vfs/StdFileProvider.h
        core/vfs/AndroidAssetsFileProvider.cpp
        core/vfs/AndroidAssetsFileProvider.h
        core/console/command/VFSCommand.cpp
        core/console/command/VFSCommand.h
        core/console/suggestion/VFSDynamicSuggestions.cpp
        core/console/suggestion/VFSDynamicSuggestions.h)

if (ANDROID)
    message(STATUS "Building for Android → Shared Library (.so)")
    add_library(GlimmerWorks SHARED ${GLIMMERWORKS_SOURCES})
else ()
    message(STATUS "Building for Desktop → Executable")
    add_executable(GlimmerWorks ${GLIMMERWORKS_SOURCES})
endif ()

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

if (ANDROID)
    target_link_libraries(GlimmerWorks PRIVATE SDL3::SDL3 SDL3_image::SDL3_image fmt::fmt SDL3_ttf::SDL3_ttf box2d::box2d log android)
else ()
    target_link_libraries(GlimmerWorks PRIVATE SDL3::SDL3 SDL3_image::SDL3_image fmt::fmt SDL3_ttf::SDL3_ttf box2d::box2d)
endif ()


target_include_directories(GlimmerWorks PRIVATE
        "${PROJECT_SOURCE_DIR}/include/json/include"
)

target_include_directories(GlimmerWorks PRIVATE
        "${PROJECT_SOURCE_DIR}/include/fastNoiseLite/Cpp"
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
if (WIN32)
    set(MAIN_FILE_NAME "GlimmerWorks.exe")
elseif (APPLE)
    set(MAIN_FILE_NAME "GlimmerWorks.app")
elseif (ANDROID)
    set(MAIN_FILE_NAME "libGlimmerWorks.so")
elseif (UNIX)
    set(MAIN_FILE_NAME "GlimmerWorks")
else ()
    set(MAIN_FILE_NAME "unknown")
endif ()
set(BUILD_SCRIPT "${CMAKE_BINARY_DIR}/build.sh")
file(WRITE "${BUILD_SCRIPT}" "#!/bin/bash\n")
file(APPEND "${BUILD_SCRIPT}" "set -e\n")
file(APPEND "${BUILD_SCRIPT}" "GAME_VERSION=\"${GAME_VERSION_STRING}\"\n")
# If not specified, Release is the default
# 如果没有指定，则默认 Release
file(APPEND "${BUILD_SCRIPT}" "BUILD_TYPE=\"${CMAKE_BUILD_TYPE}\"\n")
file(APPEND "${BUILD_SCRIPT}" "if [ -z \"\$BUILD_TYPE\" ]; then BUILD_TYPE=\"Release\"; fi\n")
# The output file name contains the version number and build type
# 输出文件名中包含版本号和构建类型
file(APPEND "${BUILD_SCRIPT}" "OUTPUT_FILE=\"GlimmerWorks_\${GAME_VERSION}_\${BUILD_TYPE}_${PLATFORM_NAME}.tar.xz\"\n")
file(APPEND "${BUILD_SCRIPT}" "echo \"Packing files into \${OUTPUT_FILE}...\"\n")
# 排除所有隐藏文件和文件夹
# Exclude all hidden files and folders
file(APPEND "${BUILD_SCRIPT}" "tar --exclude='.*' -cJf \${OUTPUT_FILE} config.json ${MAIN_FILE_NAME} langs libs mods\n")
file(APPEND "${BUILD_SCRIPT}" "echo \"Done!\"\n")

if (ANDROID)
    # Generate the install.sh installation script
    # 生成 install.sh 安装脚本
    set(INSTALL_SCRIPT "${CMAKE_BINARY_DIR}/install.sh")
    file(WRITE "${INSTALL_SCRIPT}" "#!/bin/bash\n")
    file(APPEND "${INSTALL_SCRIPT}" "set -e\n\n")
    file(APPEND "${INSTALL_SCRIPT}" "if [ -z \"\$1\" ]; then\n")
    file(APPEND "${INSTALL_SCRIPT}" "  echo \"Use: ./install.sh <AndroidProjectPath>\"\n")
    file(APPEND "${INSTALL_SCRIPT}" "  exit 1\n")
    file(APPEND "${INSTALL_SCRIPT}" "fi\n\n")
    file(APPEND "${BUILD_SCRIPT}" "GAME_VERSION=\"${GAME_VERSION_STRING}\"\n")
    file(APPEND "${BUILD_SCRIPT}" "BUILD_TYPE=\"${CMAKE_BUILD_TYPE}\"\n")
    file(APPEND "${INSTALL_SCRIPT}" "ANDROID_PROJECT_PATH=\$1\n")
    file(APPEND "${INSTALL_SCRIPT}" "JNI_LIBS_PATH=\"\$ANDROID_PROJECT_PATH/app/src/main/jniLibs/arm64-v8a\"\n")
    file(APPEND "${INSTALL_SCRIPT}" "ASSETS_PATH=\"\$ANDROID_PROJECT_PATH/app/src/main/assets\"\n\n")
    file(APPEND "${INSTALL_SCRIPT}" "echo \"Installing GlimmerWorks into: \$ANDROID_PROJECT_PATH\"\n")
    file(APPEND "${INSTALL_SCRIPT}" "mkdir -p \"\$JNI_LIBS_PATH\"\n")
    file(APPEND "${INSTALL_SCRIPT}" "mkdir -p \"\$ASSETS_PATH\"\n\n")
    file(APPEND "${INSTALL_SCRIPT}" "echo \"Copying libGlimmerWorks.so ...\"\n")
    file(APPEND "${INSTALL_SCRIPT}" "cp -v \"${CMAKE_BINARY_DIR}/libGlimmerWorks.so\" \"\$JNI_LIBS_PATH/\"\n\n")
    file(APPEND "${INSTALL_SCRIPT}" "echo \"Copying assets ...\"\n")
    file(APPEND "${INSTALL_SCRIPT}" "cp -vr \"${CMAKE_BINARY_DIR}/config.json\" \"\$ASSETS_PATH/\"\n")
    file(APPEND "${INSTALL_SCRIPT}" "cp -vr \"${CMAKE_BINARY_DIR}/langs\" \"\$ASSETS_PATH/\"\n")
    file(APPEND "${INSTALL_SCRIPT}" "
mkdir -p \"\$JNI_LIBS_PATH\"
cp -vr \"${CMAKE_BINARY_DIR}/libs\"/* \"\$JNI_LIBS_PATH/\"

# Traverse all *.so.* files under JNI_LIBS_PATH and rename them to *.so
# 遍历 JNI_LIBS_PATH 下的所有 *.so.* 文件，将其重命名为 *.so
for f in \$(find \"\$JNI_LIBS_PATH\" -type f -name '*.so.*'); do
    base=\$(echo \"\$(basename \"\$f\")\" | sed -E 's/\\.so\\..*/.so/')
    dest=\"\$(dirname \"\$f\")/\$base\"
    echo \"Renaming \$f -> \$dest\"
    mv -f \"\$f\" \"\$dest\"
done
")
    file(APPEND "${INSTALL_SCRIPT}" "cp -vr \"${CMAKE_BINARY_DIR}/mods\" \"\$ASSETS_PATH/\"\n\n")

    # Generate cppConfig.java
    # 生成 cppConfig.java
    file(APPEND "${INSTALL_SCRIPT}" "JAVA_FILE_PATH=\"\$ANDROID_PROJECT_PATH/app/src/main/java/com/coldmint/glimmerworks/CppConfig.java\"\n")
    file(APPEND "${INSTALL_SCRIPT}" "echo \"Generating \$JAVA_FILE_PATH ...\"\n")
    file(APPEND "${INSTALL_SCRIPT}" "mkdir -p \"\$(dirname \"\$JAVA_FILE_PATH\")\"\n")
    file(APPEND "${INSTALL_SCRIPT}" "cat <<EOF > \"\$JAVA_FILE_PATH\"\n")
    file(APPEND "${INSTALL_SCRIPT}" "//This file is automatically generated by the program. Please do not edit it.\npackage com.coldmint.glimmerworks;\n\n")
    file(APPEND "${INSTALL_SCRIPT}" "public class CppConfig {\n")
    file(APPEND "${INSTALL_SCRIPT}" "    public static final String BUILD_TYPE = \"${CMAKE_BUILD_TYPE}\";\n")
    file(APPEND "${INSTALL_SCRIPT}" "    public static final String GAME_VERSION = \"${GAME_VERSION_STRING}\";\n")
    file(APPEND "${INSTALL_SCRIPT}" "    public static boolean IsDebug() {\n")
    file(APPEND "${INSTALL_SCRIPT}" "        return \"Debug\".equals(BUILD_TYPE);\n")
    file(APPEND "${INSTALL_SCRIPT}" "       }\n")

    file(APPEND "${INSTALL_SCRIPT}" "}\n")
    file(APPEND "${INSTALL_SCRIPT}" "EOF\n\n")
    file(APPEND "${INSTALL_SCRIPT}" "echo \"✅ Install completed! You can now compile and run the project in Android Studio.\"\n")

    # Set execution permissions
    # 设置执行权限
    file(COPY "${INSTALL_SCRIPT}" DESTINATION "${CMAKE_BINARY_DIR}" FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ WORLD_READ)


    # Generate uninstall.sh uninstall script
    # 生成 uninstall.sh 卸载脚本
    set(UNINSTALL_SCRIPT "${CMAKE_BINARY_DIR}/uninstall.sh")
    file(WRITE "${UNINSTALL_SCRIPT}" "#!/bin/bash\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "set -e\n\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "if [ -z \"\$1\" ]; then\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "  echo \"Use: ./uninstall.sh <AndroidProjectPath>\"\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "  exit 1\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "fi\n\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "ANDROID_PROJECT_PATH=\$1\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "JNI_LIBS_PATH=\"\$ANDROID_PROJECT_PATH/app/src/main/jniLibs/arm64-v8a\"\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "ASSETS_PATH=\"\$ANDROID_PROJECT_PATH/app/src/main/assets\"\n\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "echo \"Uninstalling GlimmerWorks from: \$ANDROID_PROJECT_PATH\"\n")

    # Delete the JNI so library
    # 删除 JNI so 库
    file(APPEND "${UNINSTALL_SCRIPT}" "if [ -d \"\$JNI_LIBS_PATH\" ]; then\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "  echo \"Removing JNI libraries...\"\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "  rm -fv \"\$JNI_LIBS_PATH/libGlimmerWorks.so\"\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "  find \"\$JNI_LIBS_PATH\" -type f -name 'libSDL3*.so' -exec rm -fv {} +\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "  find \"\$JNI_LIBS_PATH\" -type f -name 'libfmt*.so' -exec rm -fv {} +\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "  find \"\$JNI_LIBS_PATH\" -type f -name 'libbox2d*.so' -exec rm -fv {} +\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "fi\n\n")

    # Delete the assets content
    # 删除 assets 内容
    file(APPEND "${UNINSTALL_SCRIPT}" "if [ -d \"\$ASSETS_PATH\" ]; then\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "  echo \"Removing assets...\"\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "  rm -rfv \"\$ASSETS_PATH/config.json\"\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "  rm -rfv \"\$ASSETS_PATH/langs\"\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "  rm -rfv \"\$ASSETS_PATH/libs\"\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "  rm -rfv \"\$ANDROID_PROJECT_PATH/app/src/main/java/com/coldmint/glimmerworks/CppConfig.java\"\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "  rm -rfv \"\$ASSETS_PATH/mods\"\n")
    file(APPEND "${UNINSTALL_SCRIPT}" "fi\n\n")

    file(APPEND "${UNINSTALL_SCRIPT}" "echo \"✅ Uninstall completed! GlimmerWorks files removed.\"\n")

    # Set execution permissions
    file(COPY "${UNINSTALL_SCRIPT}" DESTINATION "${CMAKE_BINARY_DIR}" FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ WORLD_READ)

endif ()


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
        "${CMAKE_BINARY_DIR}/libs/$<IF:$<CONFIG:Debug>,libfmtd.so.12,libfmt.so.12>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:box2d::box2d>
        "${CMAKE_BINARY_DIR}/libs/libbox2d.so"
)

#Clone data packets and resource packets.
#克隆数据包和资源包
FetchContent_Declare(
        VanillaDataPack
        GIT_REPOSITORY "https://github.com/Cold-Mint/glimmerworks-data"
        GIT_PROGRESS TRUE
        SOURCE_DIR "${CMAKE_BINARY_DIR}/mods/data_packs/vanilla"
)

FetchContent_GetProperties(VanillaDataPack)
if (NOT VanillaDataPack_POPULATED)
    FetchContent_MakeAvailable(VanillaDataPack)
endif ()

FetchContent_Declare(
        VanillaResourcePack
        GIT_REPOSITORY "https://github.com/Cold-Mint/glimmerworks-assets"
        GIT_PROGRESS TRUE
        SOURCE_DIR "${CMAKE_BINARY_DIR}/mods/resource_packs/vanilla"
)

FetchContent_GetProperties(VanillaResourcePack)
if (NOT VanillaDataPack_POPULATED)
    FetchContent_Populate(VanillaResourcePack)
endif ()
