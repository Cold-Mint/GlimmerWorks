cmake_minimum_required(VERSION 4.3)
project(GlimmerWorks)
set(CMAKE_CXX_STANDARD 26)

#Open the build log and progress output
#打开构建日志和进度输出
set(FETCHCONTENT_QUIET OFF CACHE BOOL "Show real-time git clone progress (ON=quiet, OFF=visible)" FORCE)
set(CMAKE_MESSAGE_LOG_LEVEL STATUS CACHE STRING
        "CMake message verbosity level")

include("versions.cmake")
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_BIT "64")
    set(LIB_DIR "lib64")
else ()
    set(ARCH_BIT "32")
    set(LIB_DIR "lib32")
endif ()
if (WIN32)
    #Place the dll and exe files in the same directory.
    #把dll和exe放到同级目录。
    set(LIB_DIR "")
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

math(EXPR GAME_VERSION_NUMBER
        "${CPACK_PACKAGE_VERSION_MAJOR} * 10000
   + ${CPACK_PACKAGE_VERSION_MINOR} * 100
   + ${CPACK_PACKAGE_VERSION_PATCH}"
)

set(GAME_VERSION_STRING
        "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${CMAKE_BUILD_TYPE}"
)


include(FetchContent)

option(X11_SUPPORT "Enable X11 support" OFF)
set(SDL_CAMERA OFF)
set(SDL_TRAY OFF)
# JOYSTICK
# 游戏手柄
set(SDL_JOYSTICK OFF)
set(SDL_JACK OFF)
set(SDL_DISKAUDIO OFF)
set(SDL_DUMMYAUDIO OFF)
set(SDL_HAPTIC OFF)
set(SDL_HIDAPI OFF)
# SENSOR
# 传感器支持
set(SDL_SENSOR OFF)
set(SDL_POWER OFF)
set(SDL_VIRTUAL_JOYSTICK OFF)
set(SDL_ASSERTIONS OFF)
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

set(SDLMIXER_AIFF OFF)
set(SDLMIXER_WAVE OFF)
set(SDLMIXER_VOC OFF)
set(SDLMIXER_AU OFF)
set(SDLMIXER_FLAC OFF)
set(SDLMIXER_GME OFF)
set(SDLMIXER_MOD OFF)
set(SDLMIXER_MP3 OFF)
set(SDLMIXER_MIDI OFF)
set(SDLMIXER_OPUS OFF)
set(SDLMIXER_WAVPACK OFF)
set(SDLMIXER_VORBIS_STB ON)
set(SDLMIXER_VORBIS_VORBISFILE OFF)
set(SDLMIXER_VORBIS_TREMOR OFF)
set(SDLIMAGE_BACKEND_WIC OFF)
set(SDLIMAGE_BACKEND_IMAGEIO OFF)
set(SDLIMAGE_ANI OFF)
set(SDLIMAGE_AVIF OFF)
set(SDLIMAGE_BMP OFF)
set(SDLIMAGE_GIF OFF)
set(SDLIMAGE_JPG OFF)
set(SDLIMAGE_JXL OFF)
set(SDLIMAGE_LBM OFF)
set(SDLIMAGE_PCX OFF)
set(SDLIMAGE_PNM OFF)
set(SDLIMAGE_QOI OFF)
set(SDLIMAGE_SVG OFF)
set(SDLIMAGE_TGA OFF)
set(SDLIMAGE_TIF OFF)
set(SDLIMAGE_WEBP OFF)
set(SDLIMAGE_XCF OFF)
set(SDLIMAGE_XPM OFF)
set(SDLIMAGE_XV OFF)
set(SDLIMAGE_ANI_SAVE OFF)
set(SDLIMAGE_AVIF_SAVE OFF)
set(SDLIMAGE_BMP_SAVE OFF)
set(SDLIMAGE_GIF_SAVE OFF)
set(SDLIMAGE_JPG_SAVE OFF)
set(SDLIMAGE_TGA_SAVE OFF)
set(SDLIMAGE_WEBP_SAVE OFF)

# Read and save PNG
# 读取和保存png
set(SDLIMAGE_BACKEND_STB ON)
set(SDLIMAGE_PNG_SAVE ON)
set(SDLIMAGE_PNG ON)

set(FMT_TEST OFF)
set(FMT_FUZZ OFF)
set(FMT_CUDA_TEST OFF)
set(FMT_DOC OFF)
set(FMT_INSTALL OFF)

set(BOX2D_SAMPLES OFF)
set(BOX2D_BENCHMARKS OFF)
set(BOX2D_DOCS OFF)
set(BOX2D_PROFILE OFF)
set(BOX2D_VALIDATE OFF)
set(BOX2D_UNIT_TESTS OFF)
set(BOX2D_SANITIZE OFF)

if (X11_SUPPORT)
    set(SDL_WAYLAND OFF)
    set(SDL_WAYLAND_LIBDECOR OFF)
    set(SDL_WAYLAND_SHARED OFF)
    message(STATUS "Window backend: X11 (Wayland disabled)")
endif ()
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
        SDL_mixer
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL_mixer"
        GIT_TAG ${LIBSDL-ORG_SDL_MIXER_VERSION}
        GIT_PROGRESS TRUE
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(SDL_mixer)

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
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(glimmerWorksProtobuf)

if (ANDROID)
    message(STATUS "Building for Android → Shared Library (.so)")
    add_library(GlimmerWorks SHARED)
else ()
    message(STATUS "Building for Desktop → Executable")
    add_executable(GlimmerWorks
            core/mod/dataPack/BiomeDecoratorType.cpp
            core/mod/dataPack/BiomeDecoratorType.h)
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
        core/world/generator/Chunk.cpp
        core/ecs/component/RigidBody2DComponent.cpp
        core/ecs/system/PhysicsSystem.cpp
        core/ecs/system/DebugDrawBox2dSystem.cpp
        core/utils/Box2DUtils.cpp
        core/world/generator/ChunkPhysicsHelper.cpp
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
        core/world/generator/TilePhysicsType.cpp
        core/mod/dataPack/BiomesManager.cpp
        core/world/generator/BiomeDecorator.cpp
        core/world/generator/BiomeDecoratorManager.cpp
        core/world/generator/FillBiomeDecorator.cpp
        core/console/command/LicenseCommand.cpp
        core/mod/ResourceLocator.cpp
        core/console/command/HeightMapCommand.cpp
        core/inventory/Item.cpp
        core/inventory/TileItem.cpp
        core/inventory/ItemContainer.cpp
        core/ecs/component/ItemContainerComonent.cpp
        core/ecs/system/HotBarSystem.cpp
        core/ecs/component/HotBarComonent.cpp
        core/world/generator/TileLayerType.cpp
        core/ecs/component/DroppedItemComponent.cpp
        core/ecs/system/DroppedItemSystem.cpp
        core/ecs/component/MagnetComponent.cpp
        core/ecs/component/MagneticComponent.cpp
        core/ecs/system/MagnetSystem.cpp
        core/ecs/system/AutoPickSystem.cpp
        core/ecs/component/AutoPickComponent.cpp
        core/inventory/ComposableItem.cpp
        core/inventory/ability/ItemAbility.cpp
        core/mod/dataPack/ItemManager.cpp
        core/console/command/GiveCommand.cpp
        core/console/suggestion/ComposableItemDynamicSuggestions.cpp
        core/console/suggestion/TileDynamicSuggestions.cpp
        core/ecs/component/ItemSlotComponent.cpp
        core/ecs/system/ItemSlotSystem.cpp
        core/inventory/ability/DigAbility.cpp
        core/inventory/AbilityItem.cpp
        core/console/suggestion/AbilityItemDynamicSuggestions.cpp
        core/inventory/ItemAbilityFactory.cpp
        core/ecs/component/DiggingComponent.cpp
        core/ecs/system/DiggingSystem.cpp
        core/saves/SavesManager.cpp
        core/console/command/SeedCommand.cpp
        core/ecs/system/PauseSystem.cpp
        core/ecs/component/PauseComponent.cpp
        core/utils/TimeUtils.cpp
        core/ecs/component/PlayerComponent.cpp
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
        core/ecs/system/Box2dSystemContext.cpp
        core/world/generator/SurfaceBiomeDecorator.cpp
        core/world/generator/ChunkLoader.cpp
        core/world/generator/ChunkGenerator.cpp
        core/world/generator/TerrainTileResult.cpp
        core/world/generator/TerrainResult.cpp
        core/world/generator/TerrainResultType.cpp
        core/world/structure/StaticStructureGenerator.cpp
        core/world/structure/IStructureGenerator.cpp
        core/world/structure/StructureInfo.cpp
        core/world/structure/TreeStructureGenerator.cpp
        core/mod/dataPack/StructureManager.cpp
        core/console/suggestion/StructureDynamicSuggestions.cpp
        core/console/command/PlaceCommand.cpp
        core/world/structure/StructureGeneratorManager.cpp
        core/mod/dataPack/PackDependence.cpp
        core/Hyperlink.cpp
        core/GameUIMessage.cpp
        core/inventory/InitialInventoryManager.cpp
        core/world/structure/IStructureConditionProcessor.cpp
        core/world/structure/BiomeStructureConditionProcessor.cpp
        core/world/structure/SurfaceStructureConditionProcessor.cpp
        core/world/structure/HeightStructureConditionProcessor.cpp
        core/mod/StructurePlacementConditionsManager.cpp
        core/world/structure/HorizontalSpacingStructureConditionProcessor.cpp
        core/inventory/ability/AreaMarkerAbility.cpp
        core/ecs/system/AreaMarkerSystem.cpp
        core/ecs/component/AreaMarkerComponent.cpp
        core/world/PreloadColors.cpp
        core/inventory/ContainerChangeType.cpp
        core/contributor/Contributor.cpp
        core/contributor/ContributorManager.cpp
        core/inventory/ability/MiningRangeData.cpp
        core/mod/dataPack/MobManager.cpp
        core/console/command/SummonCommand.cpp
        core/console/suggestion/CoordinateDynamicSuggestions.cpp
        core/console/suggestion/MobDynamicSuggestions.cpp
        core/ecs/system/SpiritRendererSystem.cpp
        core/ecs/component/SpiritRendererComponent.cpp
        core/shape/ShapeType.cpp
        core/shape/ShapeManager.cpp
        core/Box2dFilter.cpp
        core/ecs/component/RayCast2DComponent.cpp
        core/ecs/system/RayCast2DSystem.cpp
        core/ecs/component/MobComponent.cpp
        core/ecs/IPersistenceEntityCreator.cpp
        core/ecs/MobEntityCreator.cpp
        core/ecs/DroppedItemCreator.cpp
        core/mod/resourcePack/AudioType.cpp
        core/mod/resourcePack/AudioManager.cpp
        core/ecs/system/BiomeBGMSystem.cpp
        core/ecs/component/FloatingTextComponent.cpp
        core/ecs/system/FloatingTextSystem.cpp
        core/ecs/FlowingTextCreator.cpp
        core/ecs/IEntityCreator.cpp
        core/console/command/FlyCommand.cpp
        core/console/command/ScreenshotCommand.cpp
        core/console/command/EchoCommand.cpp
        core/console/suggestion/BooleanToggleDynamicSuggestions.cpp
        core/BoolOrToggle.cpp
        core/console/CommandHistoryManager.cpp
        core/utils/StringUtils.cpp
        core/ecs/component/TilePlacementForbiddenZoneComponent.cpp
        core/ecs/component/DraggableComponent.cpp
        core/ecs/system/DraggableSystem.cpp
        core/utils/ColorUtils.cpp
        core/ecs/system/DebugMultiMapSystem.cpp
        core/console/command/LocateCommand.cpp
        core/console/suggestion/BiomeSuggestions.cpp
        core/world/generator/MineralBiomeDecorator.cpp
        core/mod/dataPack/BiomeDecoratorResourcesManager.cpp
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
        core/world/generator/Chunk.h
        core/ecs/component/RigidBody2DComponent.h
        core/ecs/system/PhysicsSystem.h
        core/ecs/system/DebugDrawBox2dSystem.h
        core/utils/Box2DUtils.h
        core/world/generator/ChunkPhysicsHelper.h
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
        core/world/generator/TilePhysicsType.h
        core/mod/dataPack/BiomesManager.h
        core/world/generator/BiomeDecorator.h
        core/world/generator/FillBiomeDecorator.h
        core/world/generator/BiomeDecoratorManager.h
        core/console/command/LicenseCommand.h
        core/mod/ResourceLocator.h
        core/console/command/HeightMapCommand.h
        core/inventory/Item.h
        core/inventory/TileItem.h
        core/inventory/ItemContainer.h
        core/ecs/component/ItemContainerComonent.h
        core/ecs/system/HotBarSystem.h
        core/ecs/component/HotBarComonent.h
        core/world/generator/TileLayerType.h
        core/ecs/component/DroppedItemComponent.h
        core/ecs/system/DroppedItemSystem.h
        core/ecs/component/MagnetComponent.h
        core/ecs/component/MagneticComponent.h
        core/ecs/system/MagnetSystem.h
        core/ecs/system/AutoPickSystem.h
        core/ecs/component/AutoPickComponent.h
        core/inventory/ComposableItem.h
        core/inventory/ability/ItemAbility.h
        core/mod/dataPack/ItemManager.h
        core/console/command/GiveCommand.h
        core/console/suggestion/ComposableItemDynamicSuggestions.h
        core/console/suggestion/TileDynamicSuggestions.h
        core/ecs/component/ItemSlotComponent.h
        core/ecs/system/ItemSlotSystem.h
        core/inventory/ability/DigAbility.h
        core/inventory/AbilityItem.h
        core/console/suggestion/AbilityItemDynamicSuggestions.h
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
        core/ecs/system/Box2dSystemContext.h
        core/world/generator/SurfaceBiomeDecorator.h
        core/world/generator/ChunkLoader.h
        core/world/generator/ChunkGenerator.h
        core/world/generator/TerrainResult.h
        core/world/generator/TerrainResultType.h
        core/world/generator/TerrainTileResult.h
        core/world/structure/IStructureGenerator.h
        core/world/structure/StructureInfo.h
        core/world/structure/TreeStructureGenerator.h
        core/world/structure/StaticStructureGenerator.h
        core/mod/dataPack/StructureManager.h
        core/console/suggestion/StructureDynamicSuggestions.h
        core/console/command/PlaceCommand.h
        core/world/structure/StructureGeneratorManager.h
        core/mod/dataPack/PackDependence.h
        core/Hyperlink.h
        core/GameUIMessage.h
        core/inventory/InitialInventoryManager.h
        core/world/structure/IStructureConditionProcessor.h
        core/world/structure/HeightStructureConditionProcessor.h
        core/world/structure/SurfaceStructureConditionProcessor.h
        core/world/structure/BiomeStructureConditionProcessor.h
        core/mod/StructurePlacementConditionsManager.h
        core/world/structure/HorizontalSpacingStructureConditionProcessor.h
        core/inventory/ability/AreaMarkerAbility.h
        core/ecs/component/AreaMarkerComponent.h
        core/ecs/system/AreaMarkerSystem.h
        core/world/PreloadColors.h
        core/inventory/ContainerChangeType.h
        core/contributor/ContributorManager.h
        core/contributor/Contributor.h
        core/inventory/ability/MiningRangeData.h
        core/mod/dataPack/MobManager.h
        core/console/command/SummonCommand.h
        core/console/suggestion/CoordinateDynamicSuggestions.h
        core/console/suggestion/MobDynamicSuggestions.h
        core/ecs/component/SpiritRendererComponent.h
        core/ecs/system/SpiritRendererSystem.h
        core/shape/ShapeType.h
        core/shape/ShapeManager.h
        core/ecs/component/RayCast2DComponent.h
        core/ecs/system/RayCast2DSystem.h
        core/Box2dFilter.h
        core/ecs/component/MobComponent.h
        core/ecs/MobEntityCreator.h
        core/ecs/IPersistenceEntityCreator.h
        core/ecs/DroppedItemCreator.h
        core/mod/resourcePack/AudioType.h
        core/mod/resourcePack/AudioManager.h
        core/ecs/system/BiomeBGMSystem.h
        core/ecs/component/FloatingTextComponent.h
        core/ecs/system/FloatingTextSystem.h
        core/ecs/FlowingTextCreator.h
        core/ecs/IEntityCreator.h
        core/console/command/FlyCommand.h
        core/console/command/ScreenshotCommand.h
        core/console/command/EchoCommand.h
        core/console/suggestion/BooleanToggleDynamicSuggestions.h
        core/BoolOrToggle.h
        core/console/CommandHistoryManager.h
        core/utils/StringUtils.h
        core/ecs/component/TilePlacementForbiddenZoneComponent.h
        core/ecs/component/DraggableComponent.h
        core/ecs/system/DraggableSystem.h
        core/ecs/system/DebugMultiMapSystem.h
        core/utils/ColorUtils.h
        core/console/command/LocateCommand.h
        core/console/suggestion/BiomeSuggestions.h
        core/world/generator/MineralBiomeDecorator.h
        core/mod/dataPack/BiomeDecoratorResourcesManager.h
)

#Define version information
#定义版本信息
set(CONFIG_FILE_NAME "config.toml")
target_compile_definitions(GlimmerWorks PRIVATE
        CONFIG_FILE_NAME="${CONFIG_FILE_NAME}"
        GAME_VERSION_NUMBER=${GAME_VERSION_NUMBER}
        GAME_VERSION_STRING="${GAME_VERSION_STRING}"
        APP_PACKNAME="org.glimmerworks"
)

if (ANDROID)
    target_link_libraries(GlimmerWorks PRIVATE log android SDL3::SDL3 SDL3_image::SDL3_image SDL3_mixer::SDL3_mixer fmt::fmt SDL3_ttf::SDL3_ttf box2d::box2d glimmerworks::proto toml11::toml11)
else ()
    target_link_libraries(GlimmerWorks PRIVATE SDL3::SDL3 SDL3_image::SDL3_image SDL3_mixer::SDL3_mixer fmt::fmt SDL3_ttf::SDL3_ttf box2d::box2d glimmerworks::proto toml11::toml11)
endif ()


target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/fmt-src/include"
)

target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/imgui-src"
)

target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/box2d-src/include"
)

target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/toml11-src/include"
)

target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/tweeny-src/include"
)

target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/fastnoiselite-src/Cpp"
)

target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/sdl-src/include"
)
target_include_directories(GlimmerWorks PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/sdl_mixer-src/include"
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
        GIT_REPOSITORY "https://github.com/Cold-Mint/GlimmerworksData"
        GIT_PROGRESS TRUE
        SOURCE_DIR "${CMAKE_BINARY_DIR}/mods/data_packs/glimmerworks"
)

FetchContent_GetProperties(VanillaDataPack)
if (NOT VanillaDataPack_POPULATED)
    FetchContent_MakeAvailable(VanillaDataPack)
endif ()

FetchContent_Declare(
        VanillaResourcePack
        GIT_REPOSITORY "https://github.com/Cold-Mint/GlimmerworksAssets"
        GIT_PROGRESS TRUE
        SOURCE_DIR "${CMAKE_BINARY_DIR}/mods/resource_packs/glimmerworks_res"
)

FetchContent_GetProperties(VanillaResourcePack)
if (NOT VanillaDataPack_POPULATED)
    FetchContent_MakeAvailable(VanillaResourcePack)
endif ()

if (UNIX AND NOT APPLE)
    if (X11_SUPPORT)
        set(WINDOW_BACKEND_NAME "x11")
    else ()
        set(WINDOW_BACKEND_NAME "wayland")
    endif ()

    set(CPACK_PACKAGE_FILE_NAME
            "${PROJECT_NAME}-${GAME_VERSION_STRING}-Linux-${WINDOW_BACKEND_NAME}-x${ARCH_BIT}")
    set(CPACK_GENERATOR "TZST")
endif ()
if (WIN32)
    set(LIB_SUFFIX "dll")
    set(CPACK_PACKAGE_FILE_NAME
            "${PROJECT_NAME}-${GAME_VERSION_STRING}-Windows-x${ARCH_BIT}")
    set(CPACK_GENERATOR "ZIP")
else ()
    set(LIB_SUFFIX "so")
endif ()

set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/dist/${CMAKE_BUILD_TYPE}")
include(CPack)
install(TARGETS GlimmerWorks RUNTIME DESTINATION .)
install(FILES "${CMAKE_SOURCE_DIR}/${CONFIG_FILE_NAME}" DESTINATION .)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/core_langs/"
        DESTINATION "langs")
install(DIRECTORY "${CMAKE_BINARY_DIR}/mods/"
        DESTINATION "mods"
        PATTERN ".git" EXCLUDE)
install(FILES "${CMAKE_SOURCE_DIR}/LICENSE"
        DESTINATION .)

if (WIN32)
    install(CODE "
    file(GLOB PROTO_BUF_LIBS \"/usr/x86_64-w64-mingw32/sys-root/mingw/bin/*${LIB_SUFFIX}\")
    foreach(lib \${PROTO_BUF_LIBS})
        message(STATUS \"Installing \${lib} to ${LIB_DIR}\")
        file(INSTALL DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}\" FILES   \${lib})
    endforeach()
")
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/base/libabsl_base.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/base/libabsl_base.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/base/libabsl_log_severity.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/base/libabsl_log_severity.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/base/libabsl_malloc_internal.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/base/libabsl_malloc_internal.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/base/libabsl_raw_logging_internal.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/base/libabsl_raw_logging_internal.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/base/libabsl_spinlock_wait.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/base/libabsl_spinlock_wait.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/base/libabsl_strerror.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/base/libabsl_strerror.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/base/libabsl_throw_delegate.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/base/libabsl_throw_delegate.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/base/libabsl_tracing_internal.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/base/libabsl_tracing_internal.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/container/libabsl_hashtablez_sampler.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/container/libabsl_hashtablez_sampler.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/container/libabsl_raw_hash_set.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/container/libabsl_raw_hash_set.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/crc/libabsl_crc32c.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/crc/libabsl_crc32c.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/crc/libabsl_crc_cord_state.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/crc/libabsl_crc_cord_state.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/crc/libabsl_crc_cpu_detect.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/crc/libabsl_crc_cpu_detect.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/crc/libabsl_crc_internal.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/crc/libabsl_crc_internal.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/debugging/libabsl_debugging_internal.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/debugging/libabsl_debugging_internal.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/debugging/libabsl_decode_rust_punycode.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/debugging/libabsl_decode_rust_punycode.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/debugging/libabsl_demangle_internal.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/debugging/libabsl_demangle_internal.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/debugging/libabsl_demangle_rust.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/debugging/libabsl_demangle_rust.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/debugging/libabsl_examine_stack.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/debugging/libabsl_examine_stack.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/debugging/libabsl_leak_check.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/debugging/libabsl_leak_check.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/debugging/libabsl_stacktrace.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/debugging/libabsl_stacktrace.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/debugging/libabsl_symbolize.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/debugging/libabsl_symbolize.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/debugging/libabsl_utf8_for_code_point.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/debugging/libabsl_utf8_for_code_point.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/flags/libabsl_flags_commandlineflag.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/flags/libabsl_flags_commandlineflag.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/flags/libabsl_flags_commandlineflag_internal.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/flags/libabsl_flags_commandlineflag_internal.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/flags/libabsl_flags_config.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/flags/libabsl_flags_config.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/flags/libabsl_flags_internal.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/flags/libabsl_flags_internal.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/flags/libabsl_flags_marshalling.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/flags/libabsl_flags_marshalling.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/flags/libabsl_flags_private_handle_accessor.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/flags/libabsl_flags_private_handle_accessor.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/flags/libabsl_flags_program_name.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/flags/libabsl_flags_program_name.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/flags/libabsl_flags_reflection.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/flags/libabsl_flags_reflection.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/hash/libabsl_city.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/hash/libabsl_city.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/hash/libabsl_hash.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/hash/libabsl_hash.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/hash/libabsl_low_level_hash.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/hash/libabsl_low_level_hash.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_die_if_null.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_die_if_null.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_globals.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_globals.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_initialize.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_initialize.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_check_op.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_check_op.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_conditions.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_conditions.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_fnmatch.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_fnmatch.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_format.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_format.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_globals.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_globals.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_log_sink_set.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_log_sink_set.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_message.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_message.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_nullguard.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_nullguard.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_proto.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_proto.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_structured_proto.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_internal_structured_proto.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_sink.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_log_sink.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_vlog_config_internal.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/log/libabsl_vlog_config_internal.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/numeric/libabsl_int128.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/numeric/libabsl_int128.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/profiling/libabsl_exponential_biased.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/profiling/libabsl_exponential_biased.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_distributions.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_distributions.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_internal_entropy_pool.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_internal_entropy_pool.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_internal_platform.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_internal_platform.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_internal_randen.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_internal_randen.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_internal_randen_hwaes.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_internal_randen_hwaes.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_internal_randen_hwaes_impl.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_internal_randen_hwaes_impl.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_internal_randen_slow.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_internal_randen_slow.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_internal_seed_material.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_internal_seed_material.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_seed_gen_exception.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_seed_gen_exception.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_seed_sequences.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/random/libabsl_random_seed_sequences.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/status/libabsl_status.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/status/libabsl_status.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/status/libabsl_statusor.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/status/libabsl_statusor.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/strings/libabsl_cord.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/strings/libabsl_cord.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/strings/libabsl_cord_internal.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/strings/libabsl_cord_internal.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/strings/libabsl_cordz_functions.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/strings/libabsl_cordz_functions.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/strings/libabsl_cordz_handle.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/strings/libabsl_cordz_handle.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/strings/libabsl_cordz_info.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/strings/libabsl_cordz_info.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/strings/libabsl_str_format_internal.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/strings/libabsl_str_format_internal.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/strings/libabsl_string_view.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/strings/libabsl_string_view.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/strings/libabsl_strings.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/strings/libabsl_strings.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/strings/libabsl_strings_internal.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/strings/libabsl_strings_internal.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/synchronization/libabsl_graphcycles_internal.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/synchronization/libabsl_graphcycles_internal.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/synchronization/libabsl_kernel_timeout_internal.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/synchronization/libabsl_kernel_timeout_internal.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/synchronization/libabsl_synchronization.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/synchronization/libabsl_synchronization.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/time/libabsl_civil_time.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/time/libabsl_civil_time.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/time/libabsl_time.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/time/libabsl_time.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/time/libabsl_time_zone.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/absl-build/absl/time/libabsl_time_zone.${LIB_SUFFIX}.2505.0.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/box2d-build/bin/libbox2d.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/box2d-build/bin/libbox2d.${LIB_SUFFIX}.3" DESTINATION ${LIB_DIR})
    install(FILES "${CMAKE_BINARY_DIR}/_deps/box2d-build/bin/libbox2d.${LIB_SUFFIX}.3.1.1" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/fmt-build/libfmt.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/fmt-build/libfmt.${LIB_SUFFIX}.12" DESTINATION ${LIB_DIR})
    install(FILES "${CMAKE_BINARY_DIR}/_deps/fmt-build/libfmt.${LIB_SUFFIX}.12.1.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/glimmerworksprotobuf-build/libGlimmerWorksProto.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/glimmerworksprotobuf-build/libGlimmerWorksProto.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/protobuf-build/libprotobuf-lite.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/protobuf-build/libprotobuf-lite.${LIB_SUFFIX}.34.1.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/protobuf-build/third_party/utf8_range/libutf8_validity.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/protobuf-build/third_party/utf8_range/libutf8_validity.${LIB_SUFFIX}.34.1.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/sdl-build/libSDL3.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/sdl-build/libSDL3.${LIB_SUFFIX}.0" DESTINATION ${LIB_DIR})
    install(FILES "${CMAKE_BINARY_DIR}/_deps/sdl-build/libSDL3.${LIB_SUFFIX}.0.4.4" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/sdl_image-build/libSDL3_image.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/sdl_image-build/libSDL3_image.${LIB_SUFFIX}.0" DESTINATION ${LIB_DIR})
    install(FILES "${CMAKE_BINARY_DIR}/_deps/sdl_image-build/libSDL3_image.${LIB_SUFFIX}.0.4.2" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/sdl_mixer-build/libSDL3_mixer.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/sdl_mixer-build/libSDL3_mixer.${LIB_SUFFIX}.0" DESTINATION ${LIB_DIR})
    install(FILES "${CMAKE_BINARY_DIR}/_deps/sdl_mixer-build/libSDL3_mixer.${LIB_SUFFIX}.0.2.0" DESTINATION ${LIB_DIR})
endif ()

if (WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/_deps/sdl_ttf-build/libSDL3_ttf.${LIB_SUFFIX}" DESTINATION ${LIB_DIR})
else ()
    install(FILES "${CMAKE_BINARY_DIR}/_deps/sdl_ttf-build/libSDL3_ttf.${LIB_SUFFIX}.0" DESTINATION ${LIB_DIR})
    install(FILES "${CMAKE_BINARY_DIR}/_deps/sdl_ttf-build/libSDL3_ttf.${LIB_SUFFIX}.0.2.2" DESTINATION ${LIB_DIR})
endif ()

