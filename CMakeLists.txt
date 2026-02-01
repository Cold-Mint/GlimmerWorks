cmake_minimum_required(VERSION 3.28)
project(GlimmerWorks)
set(CMAKE_CXX_STANDARD 26)

#Open the build log and progress output
#打开构建日志和进度输出
set(FETCHCONTENT_QUIET OFF CACHE BOOL "Show real-time git clone progress (ON=quiet, OFF=visible)" FORCE)
set(CMAKE_MESSAGE_LOG_LEVEL STATUS CACHE STRING
        "CMake message verbosity level")

include("versions.cmake")
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(LIB_DIR "lib64")
else ()
    set(LIB_DIR "lib32")
endif ()
set(CMAKE_BUILD_RPATH "$ORIGIN/${LIB_DIR}")
set(CMAKE_INSTALL_RPATH "$ORIGIN/${LIB_DIR}")
set(CMAKE_PROJECT_HOMEPAGE_URL "https://github.com/Cold-Mint/GlimmerWorks")
set(CPACK_PACKAGE_VENDOR "Cold-Mint")
#Major version number
#主版本号
set(CPACK_PACKAGE_VERSION_MAJOR 0)
#Minor version number
#次要版本号
set(CPACK_PACKAGE_VERSION_MINOR 0)
#Patch version number
#补丁版本号
set(CPACK_PACKAGE_VERSION_PATCH 1)

set(GAME_VERSION_STRING
        "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${CMAKE_BUILD_TYPE}"
)
math(EXPR GAME_VERSION_NUMBER
        "${CPACK_PACKAGE_VERSION_MAJOR} * 10000
   + ${CPACK_PACKAGE_VERSION_MINOR} * 100
   + ${CPACK_PACKAGE_VERSION_PATCH}"
)


include(FetchContent)

option(X11_SUPPORT "Enable X11 support" OFF)
#CAMERA
#相机
set(SDL_CAMERA OFF)
#TRAY
#系统托盘
set(SDL_TRAY OFF)
set(SDL_X11 ${X11_SUPPORT})
set(SDL_X11_SHARED ${X11_SUPPORT})
set(SDL_X11_XCURSOR ${X11_SUPPORT})
set(SDL_X11_XDBE ${X11_SUPPORT})
set(SDL_X11_XFIXES ${X11_SUPPORT})
set(SDL_X11_XINPUT ${X11_SUPPORT})
set(SDL_X11_XRANDR ${X11_SUPPORT})
set(SDL_X11_XSCRNSAVER ${X11_SUPPORT})
set(SDL_X11_XSHAPE ${X11_SUPPORT})
set(SDL_X11_XSYNC ${X11_SUPPORT})
set(SDL_X11_XTEST ${X11_SUPPORT})
FetchContent_Declare(
        SDL
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
        GIT_TAG ${LIBSDL-ORG_SDL_VERSION}
        GIT_PROGRESS TRUE
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(SDL)

FetchContent_Declare(
        SDL_image
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL_image"
        GIT_TAG ${LIBSDL-ORG_SDL_IMAGE_VERSION}
        GIT_PROGRESS TRUE
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(SDL_image)


FetchContent_Declare(
        SDL_ttf
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL_ttf"
        GIT_TAG ${LIBSDL-ORG_SDL_TTF_VERSION}
        GIT_PROGRESS TRUE
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(SDL_ttf)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY "https://github.com/ocornut/imgui"
        GIT_PROGRESS TRUE
        GIT_TAG ${OCORNUT_IMGUI_VERSION}
        EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(imgui)
FetchContent_Declare(
        tweeny
        GIT_REPOSITORY "https://github.com/mobius3/tweeny"
        GIT_PROGRESS TRUE
        GIT_TAG ${MOBIUS3_TWEENY_VERSION}
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(tweeny)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY "https://github.com/fmtlib/fmt"
        GIT_PROGRESS TRUE
        GIT_TAG ${FMTLIB_FMT_VERSION}
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
        fastNoiseLite
        GIT_REPOSITORY "https://github.com/Auburn/FastNoiseLite"
        GIT_PROGRESS TRUE
        GIT_TAG ${AUBURN_FASTNOISELITE_VERSION}
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(fastNoiseLite)

FetchContent_Declare(
        box2d
        GIT_REPOSITORY "https://github.com/erincatto/box2d"
        GIT_TAG ${ERINCATTO_BOX2D_VERSION}
        GIT_PROGRESS TRUE
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(box2d)

FetchContent_Declare(
        toml11
        GIT_REPOSITORY "https://github.com/ToruNiina/toml11"
        GIT_TAG ${TORUNIINA_TOML11_VERSION}
        GIT_PROGRESS TRUE
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(toml11)

FetchContent_Declare(
        glimmerWorksProtobuf
        GIT_REPOSITORY "https://github.com/Cold-Mint/GlimmerWorksProtobuf"
        GIT_PROGRESS TRUE
        PATCH_COMMAND sed -i "s|_deps/pb-build/cmake/protobuf/protobuf-generate.cmake|_deps/pb-src/cmake/protobuf-generate.cmake|" CMakeLists.txt
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(glimmerWorksProtobuf)

if (ANDROID)
    message(STATUS "Building for Android → Shared Library (.so)")
    add_library(GlimmerWorks SHARED)
else ()
    message(STATUS "Building for Desktop → Executable")
    add_executable(GlimmerWorks)
endif ()

target_sources(GlimmerWorks
        PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/imgui-src/imgui.cpp"
        "${CMAKE_BINARY_DIR}/_deps/imgui-src/imgui_draw.cpp"
        "${CMAKE_BINARY_DIR}/_deps/imgui-src/imgui_widgets.cpp"
        "${CMAKE_BINARY_DIR}/_deps/imgui-src/imgui_tables.cpp"
        "${CMAKE_BINARY_DIR}/_deps/imgui-src/misc/cpp/imgui_stdlib.cpp"
        # SDL3 backend
        # SDL3 后端
        "${CMAKE_BINARY_DIR}/_deps/imgui-src/backends/imgui_impl_sdl3.cpp"
        "${CMAKE_BINARY_DIR}/_deps/imgui-src/backends/imgui_impl_sdlrenderer3.cpp"
        main.cpp
        core/log/LogCat.cpp
        core/Config.cpp
        core/App.cpp
        core/mod/dataPack/StringManager.cpp
        core/mod/Resource.cpp
        core/mod/dataPack/DataPackManager.cpp
        core/mod/dataPack/DataPack.cpp
        core/mod/PackManifest.cpp
        core/utils/LanguageUtils.cpp
        core/mod/ResourceRef.cpp
        core/scene/Scene.cpp
        core/scene/AppContext.cpp
        core/scene/SplashScene.cpp
        core/scene/ConsoleOverlay.cpp
        core/console/Command.cpp
        core/console/CommandExecutor.cpp
        core/console/CommandManager.cpp
        core/console/CommandArgs.cpp
        core/console/CommandResult.cpp
        core/mod/resourcePack/ResourcePack.cpp
        core/mod/resourcePack/ResourcePackManager.cpp
        core/console/command/HelpCommand.cpp
        core/scene/SceneManager.cpp
        core/scene/HomeScene.cpp
        core/Langs.cpp
        core/scene/SavedGamesScene.cpp
        core/saves/Saves.cpp
        core/saves/MapManifest.cpp
        core/scene/CreateWorldScene.cpp
        core/world/WorldContext.cpp
        core/math/Vector2D.cpp
        core/scene/WorldScene.cpp
        core/scene/DebugOverlay.cpp
        core/ecs/GameEntity.cpp
        core/ecs/GameComponent.cpp
        core/ecs/GameSystem.cpp
        core/ecs/system/Transform2DSystem.cpp
        core/ecs/component/Transform2DComponent.cpp
        core/ecs/system/GameStartSystem.cpp
        core/ecs/system/CameraSystem.cpp
        core/ecs/system/PlayerControlSystem.cpp
        core/ecs/component/CameraComponent.cpp
        core/ecs/component/DebugDrawComponent.cpp
        core/ecs/system/DebugDrawSystem.cpp
        core/ecs/component/TileLayerComponent.cpp
        core/ecs/system/TileLayerSystem.cpp
        core/world/Tile.cpp
        core/math/Vector2DI.cpp
        core/ecs/system/DebugPanelSystem.cpp
        core/ecs/system/ChunkSystem.cpp
        core/world/Chunk.cpp
        core/ecs/component/RigidBody2DComponent.cpp
        core/ecs/system/PhysicsSystem.cpp
        core/ecs/system/DebugDrawBox2dSystem.cpp
        core/utils/Box2DUtils.cpp
        core/world/ChunkPhysicsHelper.cpp
        core/console/command/TpCommand.cpp
        core/console/command/Box2DCommand.cpp
        core/utils/NodeTree.cpp
        core/console/command/AssetViewerCommand.cpp
        core/console/suggestion/DynamicSuggestionsManager.cpp
        core/console/suggestion/DynamicSuggestions.cpp
        core/console/suggestion/BoolDynamicSuggestions.cpp
        core/vfs/VirtualFileSystem.cpp
        core/vfs/FileProvider.cpp
        core/vfs/StdFileProvider.cpp
        core/vfs/AndroidAssetsFileProvider.cpp
        core/vfs/AAssetStream.cpp
        core/vfs/AAssetStreamBuf.cpp
        core/console/command/VFSCommand.cpp
        core/ecs/system/AndroidControlSystem.cpp
        core/console/suggestion/VFSDynamicSuggestions.cpp
        core/console/command/ConfigCommand.cpp
        core/console/suggestion/ConfigSuggestions.cpp
        core/mod/dataPack/TileManager.cpp
        core/world/TilePhysicsType.cpp
        core/mod/dataPack/BiomesManager.cpp
        core/world/TilePlacer.cpp
        core/world/TilePlacerManager.cpp
        core/console/command/LicenseCommand.cpp
        core/world/FillTilePlacer.cpp
        core/mod/ResourceLocator.cpp
        core/console/command/HeightMapCommand.cpp
        core/inventory/Item.cpp
        core/inventory/TileItem.cpp
        core/inventory/ItemContainer.cpp
        core/ecs/component/ItemContainerComonent.cpp
        core/ecs/system/HotBarSystem.cpp
        core/ecs/component/HotBarComonent.cpp
        core/world/TileLayerType.cpp
        core/ecs/component/DroppedItemComponent.cpp
        core/ecs/system/DroppedItemSystem.cpp
        core/ecs/component/MagnetComponent.cpp
        core/ecs/component/MagneticComponent.cpp
        core/ecs/system/MagnetSystem.cpp
        core/ecs/system/AutoPickSystem.cpp
        core/ecs/component/AutoPickComponent.cpp
        core/inventory/ComposableItem.cpp
        core/inventory/ability/ItemAbility.cpp
        core/inventory/DragAndDrop.cpp
        core/mod/dataPack/ItemManager.cpp
        core/console/command/GiveCommand.cpp
        core/console/suggestion/ComposableItemDynamicSuggestions.cpp
        core/console/suggestion/TileDynamicSuggestions.cpp
        core/ecs/component/ItemSlotComponent.cpp
        core/ecs/system/ItemSlotSystem.cpp
        core/inventory/ability/DigAbility.cpp
        core/inventory/AbilityItem.cpp
        core/console/suggestion/AbilityItemDynamicSuggestions.cpp
        core/inventory/DragState.cpp
        core/inventory/DragSourceType.cpp
        core/inventory/ItemAbilityFactory.cpp
        core/ecs/component/DiggingComponent.cpp
        core/ecs/system/DiggingSystem.cpp
        core/saves/SavesManager.cpp
        core/console/command/SeedCommand.cpp
        core/ecs/system/PauseSystem.cpp
        core/ecs/component/PauseComponent.cpp
        core/utils/TimeUtils.cpp
        core/ecs/component/PlayerControlComponent.cpp
        core/console/command/EcsCommand.cpp
        core/ecs/component/GuiTransform2DComponent.cpp
        core/ecs/component/ItemEditorComponent.cpp
        core/ecs/system/ItemEditorSystem.cpp
        core/utils/TomlUtils.cpp
        core/inventory/ability/NoneAbility.cpp
        core/lootTable/LootEntry.cpp
        core/console/command/ClearCommand.cpp
        core/lootTable/LootTableManager.cpp
        core/console/command/LootCommand.cpp
        core/console/suggestion/LootSuggestions.cpp
        PUBLIC
        FILE_SET HEADERS
        FILES
        core/log/LogCat.h
        core/Config.h
        core/App.h
        core/mod/dataPack/StringManager.h
        core/mod/Resource.h
        core/mod/dataPack/DataPackManager.h
        core/mod/dataPack/DataPack.h
        core/mod/PackManifest.h
        core/Constants.h
        core/utils/LanguageUtils.h
        core/mod/ResourceRef.h
        core/scene/Scene.h
        core/scene/AppContext.h
        core/scene/SplashScene.h
        core/scene/ConsoleOverlay.h
        core/console/Command.h
        core/console/CommandExecutor.h
        core/console/CommandManager.h
        core/console/CommandArgs.h
        core/console/CommandResult.h
        core/mod/resourcePack/ResourcePack.h
        core/mod/resourcePack/ResourcePackManager.h
        core/console/command/HelpCommand.h
        core/scene/SceneManager.h
        core/scene/HomeScene.h
        core/Langs.h
        core/scene/SavedGamesScene.h
        core/saves/Saves.h
        core/saves/MapManifest.h
        core/scene/CreateWorldScene.h
        core/world/WorldContext.h
        core/math/Vector2D.h
        core/scene/WorldScene.h
        core/scene/DebugOverlay.h
        core/ecs/GameEntity.h
        core/ecs/GameComponent.h
        core/ecs/GameSystem.h
        core/ecs/system/Transform2DSystem.h
        core/ecs/component/Transform2DComponent.h
        core/ecs/system/GameStartSystem.h
        core/ecs/component/CameraComponent.h
        core/ecs/component/DebugDrawComponent.h
        core/ecs/system/DebugDrawSystem.h
        core/ecs/component/TileLayerComponent.h
        core/ecs/system/TileLayerSystem.h
        core/world/Tile.h
        core/math/Vector2DI.h
        core/ecs/system/DebugPanelSystem.h
        core/ecs/system/ChunkSystem.h
        core/world/Chunk.h
        core/ecs/component/RigidBody2DComponent.h
        core/ecs/system/PhysicsSystem.h
        core/ecs/system/DebugDrawBox2dSystem.h
        core/utils/Box2DUtils.h
        core/world/ChunkPhysicsHelper.h
        core/console/command/TpCommand.h
        core/console/command/Box2DCommand.h
        core/utils/NodeTree.h
        core/console/command/AssetViewerCommand.h
        core/console/suggestion/DynamicSuggestionsManager.h
        core/console/suggestion/DynamicSuggestions.h
        core/console/suggestion/BoolDynamicSuggestions.h
        core/vfs/VirtualFileSystem.h
        core/vfs/FileProvider.h
        core/vfs/StdFileProvider.h
        core/vfs/AndroidAssetsFileProvider.h
        core/vfs/AAssetStream.h
        core/vfs/AAssetStreamBuf.h
        core/console/command/VFSCommand.h
        core/ecs/system/AndroidControlSystem.h
        core/console/suggestion/VFSDynamicSuggestions.h
        core/console/command/ConfigCommand.h
        core/console/suggestion/ConfigSuggestions.h
        core/mod/dataPack/TileManager.h
        core/world/TilePhysicsType.h
        core/mod/dataPack/BiomesManager.h
        core/world/TilePlacer.h
        core/world/TilePlacerManager.h
        core/console/command/LicenseCommand.h
        core/world/FillTilePlacer.h
        core/mod/ResourceLocator.h
        core/console/command/HeightMapCommand.h
        core/inventory/Item.h
        core/inventory/TileItem.h
        core/inventory/ItemContainer.h
        core/ecs/component/ItemContainerComonent.h
        core/ecs/system/HotBarSystem.h
        core/ecs/component/HotBarComonent.h
        core/world/TileLayerType.h
        core/ecs/component/DroppedItemComponent.h
        core/ecs/system/DroppedItemSystem.h
        core/ecs/component/MagnetComponent.h
        core/ecs/component/MagneticComponent.h
        core/ecs/system/MagnetSystem.h
        core/ecs/system/AutoPickSystem.h
        core/ecs/component/AutoPickComponent.h
        core/inventory/ComposableItem.h
        core/inventory/ability/ItemAbility.h
        core/inventory/DragAndDrop.h
        core/mod/dataPack/ItemManager.h
        core/console/command/GiveCommand.h
        core/console/suggestion/ComposableItemDynamicSuggestions.h
        core/console/suggestion/TileDynamicSuggestions.h
        core/ecs/component/ItemSlotComponent.h
        core/ecs/system/ItemSlotSystem.h
        core/inventory/ability/DigAbility.h
        core/inventory/AbilityItem.h
        core/console/suggestion/AbilityItemDynamicSuggestions.h
        core/inventory/DragState.h
        core/inventory/DragSourceType.h
        core/inventory/ItemAbilityFactory.h
        core/ecs/component/DiggingComponent.h
        core/ecs/system/DiggingSystem.h
        core/saves/SavesManager.h
        core/console/command/SeedCommand.h
        core/ecs/system/PauseSystem.h
        core/ecs/component/PauseComponent.h
        core/utils/TimeUtils.h
        core/console/command/EcsCommand.h
        core/ecs/component/GuiTransform2DComponent.h
        core/ecs/component/ItemEditorComponent.h
        core/ecs/system/ItemEditorSystem.h
        core/utils/TomlUtils.h
        core/inventory/ability/NoneAbility.h
        core/lootTable/LootEntry.h
        core/lootTable/LootTableManager.h
        core/console/command/ClearCommand.h
        core/console/command/LootCommand.h
        core/console/suggestion/LootSuggestions.h
)

#Define version information
#定义版本信息
set(CONFIG_FILE_NAME "config.toml")
target_compile_definitions(GlimmerWorks PRIVATE
        CONFIG_FILE_NAME="${CONFIG_FILE_NAME}"
        GAME_VERSION_NUMBER=${GAME_VERSION_NUMBER}
        GAME_VERSION_STRING="${GAME_VERSION_STRING}"
)

if (ANDROID)
    target_link_libraries(GlimmerWorks PRIVATE SDL3::SDL3 SDL3_image::SDL3_image fmt::fmt SDL3_ttf::SDL3_ttf box2d::box2d glimmerworks::proto toml11::toml11 log android)
else ()
    target_link_libraries(GlimmerWorks PRIVATE SDL3::SDL3 SDL3_image::SDL3_image fmt::fmt SDL3_ttf::SDL3_ttf box2d::box2d glimmerworks::proto toml11::toml11)
endif ()


target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/fmt-src/include"
)

target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/imgui-src"
)

target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/tweeny-src/include"
)

target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/fastnoiselite-src/Cpp"
)

target_include_directories(GlimmerWorks
        PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/generated"
)

file(COPY "${CMAKE_SOURCE_DIR}/${CONFIG_FILE_NAME}"
        DESTINATION "${CMAKE_BINARY_DIR}/")

file(COPY "${CMAKE_SOURCE_DIR}/core_langs/"
        DESTINATION "${CMAKE_BINARY_DIR}/langs/")
file(COPY "${CMAKE_SOURCE_DIR}/LICENSE"
        DESTINATION "${CMAKE_BINARY_DIR}/")

file(WRITE "${CMAKE_BINARY_DIR}/.gitignore" "*")

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

set(CPACK_GENERATOR "TZST")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/dist/${CMAKE_BUILD_TYPE}")
include(CPack)
install(TARGETS GlimmerWorks RUNTIME DESTINATION .)
install(FILES "${CMAKE_SOURCE_DIR}/${CONFIG_FILE_NAME}" DESTINATION .)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/core_langs/"
        DESTINATION "langs")
install(DIRECTORY "${CMAKE_BINARY_DIR}/mods/"
        DESTINATION "mods")
install(FILES "${CMAKE_SOURCE_DIR}/LICENSE"
        DESTINATION .)

install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/sdl_image-build/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")


install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/fmt-build/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")

install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/sdl_ttf-build/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")

install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/box2d-build/bin/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")

install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")

install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/absl-build/absl/flags/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")

install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/absl-build/absl/debugging/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")

install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/absl-build/absl/container/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")

install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/absl-build/absl/hash/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")

install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")

install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/absl-build/absl/status/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")

install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/absl-build/absl/strings/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")

install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/absl-build/absl/profiling/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")

install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/absl-build/absl/crc/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")
install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/absl-build/absl/base/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")

install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/absl-build/absl/synchronization/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")
install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/absl-build/absl/time/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")
install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/absl-build/absl/numeric/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")

install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/pb-build/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")

install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/pb-build/third_party/utf8_range/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")

install(CODE "
    file(GLOB PROTO_BUF_LIBS \"${CMAKE_BINARY_DIR}/_deps/glimmerworksprotobuf-build/*so*\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")